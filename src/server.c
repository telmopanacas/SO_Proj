#include <stdio.h>
#include "client.h"
#include "main.h"
#include "memory-private.h"
#include "memory.h"
#include "process.h"
#include "proxy.h"
#include "server.h"


/* Função principal de um Servidor. Deve executar um ciclo infinito onde em 
* cada iteração do ciclo lê uma operação dos proxies e se a mesma tiver id 
* diferente de -1 e se data->terminate ainda for igual a 0, processa-a e
* escreve a resposta para os clientes. Operações com id igual a -1 são 
* ignoradas (op inválida) e se data->terminate for igual a 1 é porque foi 
* dada ordem de terminação do programa, portanto deve-se fazer return do
* número de operações processadas. Para efetuar estes passos, pode usar os
* outros métodos auxiliares definidos em server.h.
*/
int execute_server(int server_id, struct communication_buffers* buffers, struct main_data* data){
    struct operation op;
    struct operation *op_ptr = &op;

    while (1)
    {
        server_receive_operation(op_ptr,buffers,data);
        if (op_ptr->id != -1 && *data->terminate == 0)
        {
            server_process_operation(op_ptr, server_id, data->server_stats);
            server_send_answer(op_ptr,buffers, data);
        }

        if (*data->terminate == 1)
        {
            return *data->server_stats;
        }
    }
    
}

    
/* Função que lê uma operação do buffer de memória partilhada entre
* proxies e servidores. Antes de tentar ler a operação, deve verificar 
* se data->terminate tem valor 1. Em caso afirmativo, retorna imediatamente 
* da função.
*/
void server_receive_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data){
    if(*data->terminate == 1){
        //Baza
        return;
    }
    read_rnd_access_buffer(buffers->prx_srv, data->buffers_size, op);
}
/* Função que processa uma operação, alterando o seu campo server para o id
* passado como argumento, alterando o estado da mesma para 'S' (served), e 
* incrementando o contador de operações.
*/
void server_process_operation(struct operation* op, int proxy_id, int* counter){
    //Perguntar ao naercio
    op->server = proxy_id;
    op->status = 'S';
    *counter+=1;
}
/* Função que escreve uma operação no buffer de memória partilhada entre
* servidores e clientes.
*/
void server_send_answer(struct operation* op, struct communication_buffers* buffers, struct main_data* data){
     write_circular_buffer(buffers->srv_cli, data->buffers_size, op);
}
