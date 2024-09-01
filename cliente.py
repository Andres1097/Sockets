import socket

SERVER_IP = "127.0.0.1"  # Dirección IP del servidor (localhost)
PORT = 8080
BUFFER_SIZE = 1024

def main():
    # Crear el socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    try:
        # Conectar al servidor
        client_socket.connect((SERVER_IP, PORT))
        print("Conectado al servidor")
        
        # Recibir el menú del servidor
        menu = client_socket.recv(BUFFER_SIZE).decode('utf-8')
        print(f"{menu}\n")
        
        while True:
            # Leer mensaje del usuario
            send_buffer = input("Ingrese mensaje para enviar: ").strip()
            
            # Enviar mensaje al servidor
            client_socket.sendall(send_buffer.encode('utf-8'))
            
            # Salir si el usuario eligió "0"
            if send_buffer == "0":
                break
            
            # Recibir respuesta del servidor
            recv_buffer = client_socket.recv(BUFFER_SIZE).decode('utf-8')
            if recv_buffer:
                print(f"\n{recv_buffer}\n")
            else:
                print("Conexión cerrada por el servidor")
                break
    except Exception as e:
        print(f"Error: {e}")
    finally:
        # Cerrar el socket
        client_socket.close()

if __name__ == "__main__":
    main()
