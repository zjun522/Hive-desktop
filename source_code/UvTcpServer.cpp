#include "UvTcpServer.h"


uv_loop_t* UvTcpServer::loop;
struct sockaddr_in UvTcpServer::addr;
QHash<UvTcpServer::SocketDescriptor, HiveProtocol::HiveClient*> UvTcpServer::buffer_hash;
QHash<QString, UvTcpServer::SocketDescriptor> UvTcpServer::key_sd_hash;



bool
HiveProtocol::readTcp(const QString &data, HiveClient *clientBuffer) //recursion decode
{
  qDebug()<<"\n\n";
  Log::net(Log::Normal, "Bee::read()", "New READ section begins");
  if(data.isEmpty() && clientBuffer->buffer.isEmpty())
    {
      Log::net(Log::Error, "Bee:read()", "data empty");
      return false;
    }

  Log::net(Log::Normal, "Bee::read()", "Stream: " + data);
  Log::net(Log::Normal, "Bee::read()", "Current Buffer Size: " + QString::number( clientBuffer->buffer.size()));

  clientBuffer->buffer.append(data);

  //if size header is 0
  if(clientBuffer->readSize == 0)
    {
      //if 16 digit size header is not complete, return
      if(clientBuffer->buffer.size() < 16)
        {
          Log::net(Log::Normal, "Bee::read()", "Failed: value \"size\" in header is not complete");
          return false;
        }
      else
        {
          clientBuffer->readSize = clientBuffer->buffer.mid(0, 16).toInt();
          clientBuffer->buffer.remove(0, 16);
          Log::net(Log::Normal, "Bee::read()", "Member clientBuffer->readSize is set to " + QString::number(clientBuffer->readSize));
        }
    }

  //if data is not complete, return
  if(clientBuffer->buffer.size() < clientBuffer->readSize)
    {
      Log::net(Log::Normal, "Bee::read()", "Failed: buffer not filled.");
      return false;
    }
  else //else read
    {
      QString packet = clientBuffer->buffer.mid(0, clientBuffer->readSize - 1);
      clientBuffer->buffer.remove(0, clientBuffer->readSize - 1);
      clientBuffer->readSize = 0;

      Log::net(Log::Normal, "Bee::read()", "Get packet: " + packet);

      if(!decodePacket(packet))
        {
          Log::net(Log::Error, "bool Bee::readBuffer()", "Packet decode failed!");
          clientBuffer->buffer.clear();

          //have to reset the connection!

          return false;
        }
    }


  return readTcp("", clientBuffer);
}

bool
HiveProtocol::writeTcp(const MessageType &MsgType, const QString &data)
{

}

bool
HiveProtocol::decodePacket(const QString &data)
{
  QByteArray byteArray = data.toLatin1();
  QJsonParseError jsonError;
  QJsonDocument readJsonDocument = QJsonDocument::fromJson(byteArray, &jsonError);
  if(jsonError.error != QJsonParseError::NoError && !readJsonDocument.isObject())
    {
      Log::net(Log::Critical, "HiveProtocol::decodePacket()", QString(jsonError.errorString() + " in stream: " + data));
      return false;
    }

  QJsonObject packetJson = readJsonDocument.object();
  QString receiverKey = packetJson.value("receiver").toString();
  if(receiverKey != GlobalData::settings_struct.profile_key_str)
    {
      Log::net(Log::Error, "HiveProtocol::decodePacket()", "Package delivered to wrong person!\n\t"+packetJson.value("receiver").toString() + "\n" + GlobalData::settings_struct.profile_key_str );
      return false;
    }
  MessageType messageType = (MessageType)packetJson.value("msgType").toInt();
  switch (messageType) {
    case MessageType::FileInfo:
      {
        Log::net(Log::Normal, "HiveConnection::decodePacket()", "File info received.");

        break;
      }
    case MessageType::FileContent:
      {

        break;
      }
    case MessageType::FileReject:
      {

        break;
      }
    case MessageType::FileAccept:
      {

        break;
      }
    case MessageType::ErrorDelivery:
      {

        break;
      }
  }
  return true;
}

bool
HiveProtocol::processHeartBeat(const UsrProfileStruct &usrProfileStruct)
{
  if(usrProfileStruct.key.isEmpty())
    {
      return;
    }

  if(usrProfileStruct.key == GlobalData::settings_struct.profile_key_str)
    {
      if(GlobalData::g_localHostIP != usrProfileStruct.ip)
        {
          GlobalData::g_localHostIP = usrProfileStruct.ip;
        }
      Log::net(Log::Normal, "NetworkManager::udpProcessHeartBeat()", "got heart beat from myself");
//      emit usrEnter(usrProfileStruct); << FIX HERE!!
    }
  else
    {
      Log::net(Log::Normal, "NetworkManager::udpProcessHeartBeat()", "got heart beat from others");
//      emit usrEnter(usrProfileStruct); << FIX HERE!!
    }
}

bool
HiveProtocol::processUsrLeave(QString *usrKey)
{
  if(*usrKey == GlobalData::settings_struct.profile_key_str)
    {
//      emit usrLeft(usrKey); << FIX HERE!!

      qDebug()<<"@NetworkManager::udpProcessUsrLeft(): Myself left.";
    }

  qDebug()<<"@NetworkManager::udpProcessUsrLeft(): Someone left.";
//  emit usrLeft(usrKey); << FIX HERE!!
}

bool
HiveProtocol::processErrorDelivery()
{

}

bool
HiveProtocol::processMessage()
{
//  if(messageStruct.sender.isEmpty() || messageStruct.reciever.isEmpty())
//    {
//      return;
//    }

//  if(messageStruct.reciever != GlobalData::settings_struct.profile_key_str)
//    {
//      if(messageStruct.sender != GlobalData::settings_struct.profile_key_str)
//        {
//          //no sniffing man!
//          return;
//        }
//      else
//        {
//          qDebug()<<"@NetworkManager::udpProcessMessage(): Got msg I sent: "<<messageStruct.message;
//          emit messageRecieved(messageStruct, true);
//        }
//    }
//  else
//    {
//      if(messageStruct.sender == GlobalData::settings_struct.profile_key_str)
//        {
//          qDebug()<<"@NetworkManager::udpProcessMessage(): me 2 me...";
//          emit messageRecieved(messageStruct, true);
//        }
//      else
//        {
//          qDebug()<<"@NetworkManager::udpProcessMessage(): Other people sent: "<<messageStruct.message;
//          emit messageRecieved(messageStruct, false);
//        }
//    }
}

bool
HiveProtocol::processFileInfo()
{

}

bool
HiveProtocol::processFileContent()
{

}

bool
HiveProtocol::processFileAccept()
{

}

bool
HiveProtocol::processFileReject()
{

}









UvTcpServer::UvTcpServer(QObject *parent) : QThread(parent)
{
}

UvTcpServer::~UvTcpServer()
{
}

void
UvTcpServer::closeUvLoop()
{
  uv_stop(loop);
  Log::net(Log::Normal, "UvTcpServer::closeUvLoop()", "Successfully closed uv event loop.");
}

void
UvTcpServer::run()
{
  qDebug()<<"uv thread id: "<<this->currentThreadId();
  Log::net(Log::Normal, "UvTcpServer::run()", "Thread Started");

  loop = uv_default_loop();

  uv_tcp_t server;
  uv_tcp_init(loop, &server);
  uv_ip4_addr("0.0.0.0", TCP_PORT, &addr);
  uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
  int r = uv_listen((uv_stream_t*)&server, TCP_BACKLOG, onNewConnection);
  if(r)
    {
      Log::net(Log::Error, "UvTcpServer::run()", QString("Listen error: " + QString(uv_strerror(r))));
      fprintf(stderr, "Listen error %s\n", uv_strerror(r));
    }
  uv_run(loop, UV_RUN_DEFAULT);

  Log::net(Log::Normal, "UvTcpServer::run()", "Quit Thread");

}

void
UvTcpServer::onNewConnection(uv_stream_t *server, int status)
{
  if(status < 0)
    {
      fprintf(stderr, "New connection error %s\n", uv_strerror(status));
      return;
    }

  uv_tcp_t *client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
  uv_tcp_init(loop, client);


  if(uv_accept(server, (uv_stream_t*)client) == 0)
    {
      uv_read_start((uv_stream_t*)client, allocBuffer, tcpRead);
    }
  else
    {
      uv_close((uv_handle_t*) client, NULL);
    }
}

void
UvTcpServer::tcpRead(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
{
  if (nread > 0)
    {
      SocketDescriptor socketDiscriptor = getSocketDescriptor(client);
      HiveClient *hiveClient;
      if(buffer_hash.contains(socketDiscriptor))
        {
          Log::net(Log::Normal, "UvTcpServer::tcpRead()", "Reading message form old user: " + QString::number(socketDiscriptor));
          hiveClient = buffer_hash.value(socketDiscriptor);
        }
      else
        {
          Log::net(Log::Normal, "UvTcpServer::tcpRead()", "Reading message form new user: " + QString::number(socketDiscriptor));
          hiveClient = new HiveClient();
          buffer_hash.insert(socketDiscriptor, hiveClient);
        }

      uv_buf_t buffer = uv_buf_init(buf->base, nread);
      readTcp(QString::fromUtf8(buffer.base, buffer.len), hiveClient);

      write_req_t *req = (write_req_t*)malloc(sizeof(write_req_t));
      req->buf = uv_buf_init(buf->base, nread);

      uv_write((uv_write_t*)req, client, &req->buf, 1, tcpWrite);

      return;
    }
  if (nread < 0) {
      if (nread != UV_EOF)
        {
          fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        }
      int socketDiscriptor = getSocketDescriptor(client);
      Log::net(Log::Normal, "UvTcpServer::tcpRead()", "Disconnected from discriptor: " + QString::number(socketDiscriptor));

      uv_close((uv_handle_t*)client, NULL); // NULL is a close callback
    }

  free(buf->base);
}

void
UvTcpServer::tcpWrite(uv_write_t *req, int status)
{
  if (status)
    {
      fprintf(stderr, "Write error %s\n", uv_strerror(status));
    }
  freeWriteReq(req);
}

void
UvTcpServer::allocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
  buf->base = (char*) malloc(suggested_size);
  buf->len = suggested_size;
}

void
UvTcpServer::freeWriteReq(uv_write_t *req)
{
  write_req_t *wr = (write_req_t*) req;
  free(wr->buf.base);
  free(wr);
}

int
UvTcpServer::getSocketDescriptor(uv_stream_t *handle)
{
  int fd;
  uv_fileno((uv_handle_t *)handle, &fd);
  return fd;
}







