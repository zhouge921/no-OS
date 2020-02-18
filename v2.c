//irq.h
enum UART_EVENT {
	UART_WRITE_DONE		= 0x0,
	UART_READ_DONE		= 0x1,
	UART_DATA_AVAILABLE	= 0x2,
    UART_ERROR          = 0x3
}

typedef void (*IRQ_CALLBACK)(void *context, uint32_t event, void *extra);
int32_t irq_register_callback(struct irq_ctrl_desc *desc, uint32_t irq_id,
		     IRQ_CALLBACK callback, void *context);

//irq.c



int32_t irq_register_callback(struct irq_ctrl_desc *desc, uint32_t irq_id,
		     IRQ_CALLBACK callback, void *context) {
                 if (irq_id == UART_ID){
                    internla_irq_register(*desc, irq_id, default_uart_irq_handler, context);
                    platform_register_callback(callback);
                 }
                    

             }

//uart_extra.c
void default_uart_irq_handler(void *context){
    flags = read_hw_flags();
    switch (flags){
        case 1:user_callback(context, UART_WRITE_DONE,0);
        case 2:user_callback(context, UART_READ_DONE,0);
        //...
    }
    clear_hw_flags();
}

//main.c
void app_callback(void *context, uint32_t event, void *extra)
{
    switch (event)
        //DO something
}

void  main(){
    irq_register_callback(desc, id, app_callback, app_context);
}