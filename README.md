Criptfly

Descrição:
Criptfly é um programa de chat seguro que permite comunicação simultânea entre cliente e servidor em rede local ou mundial. Desenvolvido em C, com foco em aprendizado de criptografia e redes.

Funcionalidades atuais

Cliente e servidor no mesmo programa.

Troca de mensagens simultânea (não bloqueante).

Escolha de rede local ou internet (input de IP e porta).

Registro mínimo de status de mensagens.

Funcionalidades futuras

Modularização do código.

Arquivo de configuração separado (host, porta, chave).

Confirmação de entrega (ACK).

Suporte a envio de arquivos pequenos.

Criptografia real (AES, ChaCha20) e verificação de integridade.

Versão Linux/Termux.

Como executar

Compile o código com:

gcc criptfly.c -o criptfly -lws2_32   # Windows
gcc criptfly.c -o criptfly             # Linux


Execute:

./criptfly


Siga as instruções do menu para escolher modo (cliente/servidor) e rede.

Observações

Este projeto é educacional.

Não utilizar para fins maliciosos.
