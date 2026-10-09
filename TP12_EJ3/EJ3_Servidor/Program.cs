using System.Net;
using System.Net.Sockets;
using System.Text;

const int puerto = 13001;

var listener = new TcpListener(IPAddress.Any, puerto);
listener.Start();
Console.WriteLine($"Servidor TCP escuchando en el puerto {puerto}...");

while (true)
{
    Console.WriteLine("Esperando una conexión TCP...");
    using TcpClient cliente = listener.AcceptTcpClient();   // bloquea hasta que llega un cliente
    Console.WriteLine($"¡Cliente conectado! ({cliente.Client.RemoteEndPoint})");

    using NetworkStream stream = cliente.GetStream();
    using var lector = new BinaryReader(stream, Encoding.UTF8);
    using var escritor = new BinaryWriter(stream, Encoding.UTF8);

    try
    {
        while (true)
        {
            string mensaje = lector.ReadString();            // lee un mensaje completo
            Console.WriteLine($"Recibido: {mensaje}");

            if (mensaje.Trim().ToLower() == "salir")
            {
                escritor.Write("Conexión cerrada por el servidor");
                escritor.Flush();
                break;
            }

            escritor.Write($"ACK: mensaje recibido correctamente ({mensaje.Length} caracteres)");
            escritor.Flush();
        }
    }
    catch (EndOfStreamException)
    {
        Console.WriteLine("El cliente cerró la conexión.");
    }
    catch (IOException ex)
    {
        Console.WriteLine($"Se perdió la conexión: {ex.Message}");
    }
}