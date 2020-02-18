//irq.h
int32_t irq_register(struct irq_ctrl_desc *desc, uint32_t irq_id,
		     void (*irq_handler)(void *context), void *context);
//uart.h
enum UART_EVENT {
	UART_WRITE_DONE		= 0x0,
	UART_READ_DONE		= 0x1,
	UART_DATA_AVAILABLE	= 0x2,
    UART_ERROR          = 0x3
}
typedef void (*UART_CALLBACK)(void *context, uint32_t event, void *extra);

int32_t uart_register_callback(struct uart_desc *desc, UART_CALLBACK callback,
                                                                void *context);
void default_uart_irq_handler(void *context);

//uart.c
void default_uart_irq_handler(void *context){
    flags = read_hw_flags();
    switch (flags){
        case 1:user_callback(context, UART_WRITE_DONE,0);
        case 2:user_callback(context, UART_READ_DONE,0);
        //...
    }
    clear_hw_flags();
}

int32_t uart_register_callback(struct uart_desc *desc, UART_CALLBACK callback,
                                                                void *context)
{
    //implementation
}

//main.c
void app_callback(void *context, uint32_t event, void *extra)
{
    switch (event)
        //DO something
}

void  main(){
    irq_register(desc, id, default_uart_irq_handler, 0);
    uart_register_callback(u_desc, app_callback, app_context);
}