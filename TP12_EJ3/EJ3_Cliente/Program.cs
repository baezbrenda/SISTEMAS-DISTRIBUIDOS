using System.Net.Sockets;
using System.Text;

string ip = args.Length > 0 ? args[0] : "127.0.0.1";
const int puerto = 13001;

try
{
    using var cliente = new TcpClient();
    cliente.Connect(ip, puerto);                              // conexión (handshake de TCP)
    Console.WriteLine($"Conectado al servidor {ip}:{puerto}");

    using NetworkStream stream = cliente.GetStream();
    using var lector = new BinaryReader(stream, Encoding.UTF8);
    using var escritor = new BinaryWriter(stream, Encoding.UTF8);

    while (true)
    {
        Console.Write("Mensaje (escribí 'salir' para terminar): ");
        string? linea = Console.ReadLine();
        if (string.IsNullOrWhiteSpace(linea)) continue;

        escritor.Write(linea);                                // envía el mensaje con su largo
        escritor.Flush();
        Console.WriteLine($"Enviado: {linea}");

        string respuesta = lector.ReadString();               // espera la confirmación del servidor
        Console.WriteLine($"Respuesta del servidor: {respuesta}");

        if (linea.Trim().ToLower() == "salir") break;
    }
}
catch (SocketException ex)
{
    Console.WriteLine($"No se pudo conectar al servidor: {ex.Message}");
}
catch (EndOfStreamException)
{
    Console.WriteLine("El servidor cerró la conexión.");
}