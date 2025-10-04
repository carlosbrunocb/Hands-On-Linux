#include <linux/module.h>
#include <linux/usb.h>
#include <linux/slab.h>

MODULE_AUTHOR("DevTITANS <devtitans@icomp.ufam.edu.br>");
MODULE_DESCRIPTION("Driver de acesso ao SmartLamp (ESP32 com Chip Serial CP2102)");
MODULE_LICENSE("GPL");


#define MAX_RECV_LINE 100 // Tamanho máximo de uma linha de resposta do dispositivo USB

static char recv_line[MAX_RECV_LINE];              // Buffer para armazenar linha completa recebida
static struct usb_device *smartlamp_device;        // Referência para o dispositivo USB
static uint usb_in, usb_out;                       // Endereços das portas de entrada e saida da USB
static char *usb_in_buffer, *usb_out_buffer;       // Buffers de entrada e saída da USB
static int usb_max_size;                           // Tamanho máximo de uma mensagem USB

#define VENDOR_ID   0x10C4 /* Encontre o VendorID  do smartlamp */
#define PRODUCT_ID  0xEA60 /* Encontre o ProductID do smartlamp */
static const struct usb_device_id id_table[] = { { USB_DEVICE(VENDOR_ID, PRODUCT_ID) }, {} };

static int  usb_probe(struct usb_interface *ifce, const struct usb_device_id *id); // Executado quando o dispositivo é conectado na USB
static void usb_disconnect(struct usb_interface *ifce);                            // Executado quando o dispositivo USB é desconectado da USB
static int  usb_write_serial(char *cmd, int param);                                // Executado para enviar um comando para o dispositivo
static int  usb_read_serial(void);                                                 // Executado para ler uma linha completa da porta serial

// Função para configurar os parâmetros seriais do CP2102 via Control-Messages
static int smartlamp_config_serial(struct usb_device *dev)
{
    int ret;
    u32 baudrate = 115200; // Defina o baud rate que seu ESP32 usa!

    printk(KERN_INFO "SmartLamp: Configurando a porta serial...\n");

    // 1. Habilita a interface UART do CP2102
    //    Comando específico do vendor Silicon Labs (CP210X_IFC_ENABLE)
    //    bmRequestType: 0x41 (Vendor, Host-to-Device, Interface)
    //    bRequest: 0x00 (CP210X_IFC_ENABLE)
    //    wValue: 0x0001 (UART Enable)
    ret = usb_control_msg(dev, usb_sndctrlpipe(dev, 0),
                          0x00, 0x41, 0x0001, 0, NULL, 0, 1000);
    if (ret)
    {
        printk(KERN_ERR "SmartLamp: Erro ao habilitar a UART (código %d)\n", ret);
        return ret;
    }

    // 2. Define o baud rate
    //    Comando específico do vendor Silicon Labs (CP210X_SET_BAUDRATE)
    //    bRequest: 0x1E (CP210X_SET_BAUDRATE)
    ret = usb_control_msg(dev, usb_sndctrlpipe(dev, 0),
                          0x1E, 0x41, 0, 0, &baudrate, sizeof(baudrate), 1000);
    if (ret < 0)
    {
        printk(KERN_ERR "SmartLamp: Erro ao configurar o baud rate (código %d)\n", ret);
        return ret;
    }

    printk(KERN_INFO "SmartLamp: Baud rate configurado para %d\n", baudrate);
    return 0;
}

MODULE_DEVICE_TABLE(usb, id_table);
bool ignore = true;

static struct usb_driver smartlamp_driver = {
    .name        = "smartlamp",     // Nome do driver
    .probe       = usb_probe,       // Executado quando o dispositivo é conectado na USB
    .disconnect  = usb_disconnect,  // Executado quando o dispositivo é desconectado na USB
    .id_table    = id_table,        // Tabela com o VendorID e ProductID do dispositivo
};

module_usb_driver(smartlamp_driver);

// Executado quando o dispositivo é conectado na USB
static int usb_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    struct usb_endpoint_descriptor *usb_endpoint_in, *usb_endpoint_out;
    int ret;

    printk(KERN_INFO "SmartLamp: Dispositivo conectado ...\n");

    // Detecta portas e aloca buffers de entrada e saída de dados na USB
    smartlamp_device = interface_to_usbdev(interface);
    ignore =  usb_find_common_endpoints(interface->cur_altsetting, &usb_endpoint_in, &usb_endpoint_out, NULL, NULL);
    usb_max_size = usb_endpoint_maxp(usb_endpoint_in);
    usb_in = usb_endpoint_in->bEndpointAddress;
    usb_out = usb_endpoint_out->bEndpointAddress;
    usb_in_buffer = kmalloc(usb_max_size, GFP_KERNEL);
    usb_out_buffer = kmalloc(usb_max_size, GFP_KERNEL);

    // Chama a função para configurar a porta serial antes de usar
    ret = smartlamp_config_serial(smartlamp_device);
    if (ret)
    {
        printk(KERN_ERR "SmartLamp: Falha na configuração da serial\n");
        kfree(usb_in_buffer);
        kfree(usb_out_buffer);
        return ret;
    }

    // TASK 2.3: Leitura de dados periódicos enviados pelo firmware
    // O firmware envia RES GET_LDR Z automaticamente a cada 2 segundos
    // Descomente as linhas abaixo após implementar usb_read_serial
    ret = usb_read_serial();
    if (ret >= 0) {
        printk(KERN_INFO "SmartLamp: Valor do LDR recebido: %d\n", ret);
    }

    return 0;
}

// Executado quando o dispositivo USB é desconectado da USB
static void usb_disconnect(struct usb_interface *interface) {
    printk(KERN_INFO "SmartLamp: Dispositivo desconectado.\n");
    kfree(usb_in_buffer);                   // Desaloca buffers
    kfree(usb_out_buffer);
}

// Envia um comando para o dispositivo USB
// Exemplo de uso: usb_write_serial("SET_LED", 80);
// Exemplo de uso: usb_write_serial("GET_LDR", 0);
static int usb_write_serial(char *cmd, int param) {
    int ret, actual_size;

    printk(KERN_INFO "SmartLamp: Enviando comando: %s %d\n", cmd, param);

    // Formata o comando no buffer
    sprintf(usb_out_buffer, "%s %d\n", cmd, param);

    // Envia o comando pela porta serial
    ret = usb_bulk_msg(smartlamp_device, usb_sndbulkpipe(smartlamp_device, usb_out),
                       usb_out_buffer, strlen(usb_out_buffer), &actual_size, 1000);
    if (ret) {
        printk(KERN_ERR "SmartLamp: Erro ao enviar comando (código %d)\n", ret);
        return -1;
    }

    printk(KERN_INFO "SmartLamp: Comando enviado com sucesso\n");
    return 0;
}

// Lê uma linha completa da porta serial (até encontrar '\n')
// Retorna o valor numérico da resposta ou -1 em caso de erro
// Exemplo de resposta: "RES GET_LDR 450\n" -> retorna 450
// Exemplo de resposta: "RES SET_LED 1\n" -> retorna 1
static int usb_read_serial(void) {
    int ret, actual_size;
    int recv_size = 0;  // Quantidade de caracteres já recebidos em recv_line
    int i;
    int value = -1;     // Valor numérico a ser retornado
    char *p;

    printk(KERN_INFO "SmartLamp: Aguardando resposta do dispositivo...\n");

    // Loop para ler dados em blocos até encontrar '\n'
    while (recv_size < MAX_RECV_LINE - 1) { // -1 para garantir espaço para o '\0'
        // Tenta ler um bloco de dados da porta serial
        // O timeout de 2000ms (2s) é usado para aguardar a chegada dos dados
        ret = usb_bulk_msg(smartlamp_device, usb_rcvbulkpipe(smartlamp_device, usb_in),
                            usb_in_buffer, usb_max_size, &actual_size, 2100);

        if (ret) {
            // Se houver erro ou timeout (retorna !0)
            if (ret == -ETIMEDOUT) {
                printk(KERN_WARNING "SmartLamp: Timeout de leitura (%dms)\n", 2000);
            } else {
                printk(KERN_ERR "SmartLamp: Erro ao ler da serial (código %d)\n", ret);
            }
            return -1;
        }

        // Se actual_size for 0, não há dados, continua o loop
        if (actual_size == 0) {
            continue;
        }
        printk(KERN_INFO "SmartLamp: DEBUG -> read_buffer: %s\n", usb_in_buffer);

        // Copia os dados lidos (actual_size bytes) do usb_in_buffer para o recv_line
        // Deve-se garantir que não exceda o tamanho máximo do buffer
        int bytes_to_copy = min(actual_size, MAX_RECV_LINE - 1 - recv_size);
        memcpy(recv_line + recv_size, usb_in_buffer, bytes_to_copy);
        recv_size += bytes_to_copy;

        // Adiciona o terminador nulo temporariamente para busca
        recv_line[recv_size] = '\0';

        // Procura pelo caractere de nova linha ('\n') no buffer acumulado
        p = strchr(recv_line, '\n');
        if (p) {
            // Linha completa encontrada!
            // Remove o '\n' e substitui por '\0' para encerrar a string
            *p = '\0';

            printk(KERN_INFO "SmartLamp: Linha completa recebida: \"%s\"\n", recv_line);

            // Tenta extrair o valor numérico da resposta
            // O formato esperado é "RES CMD_NAME VALUE"
            // Por exemplo: "RES GET_LDR 450"
            if (sscanf(recv_line, "RES %*s %d", &value) == 1) {
                // Leitura bem-sucedida do valor
                return value;
            } else {
                // Formato de resposta inválido
                printk(KERN_ERR "SmartLamp: Formato de resposta inválido: \"%s\"\n", recv_line);
                return -1;
            }
        }
    }

    // Se o loop terminou por overflow de buffer
    if (recv_size >= MAX_RECV_LINE - 1) {
        printk(KERN_ERR "SmartLamp: Buffer Overflow na leitura serial. Linha muito longa.\n");
    }

    return -1; // Retorna -1 em caso de erro, timeout, ou linha muito longa
}
