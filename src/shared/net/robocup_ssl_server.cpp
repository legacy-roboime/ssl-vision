//========================================================================
//  This software is free: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License Version 3,
//  as published by the Free Software Foundation.
//
//  This software is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  Version 3 in the file COPYING that came with this distribution.
//  If not, see <http://www.gnu.org/licenses/>.
//========================================================================
/*!
  \file    robocup_ssl_server.cpp
  \brief   C++ Implementation: robocup_ssl_server
  \author  Stefan Zickler, 2009
  \author  Jan Segre, 2012
*/
//========================================================================
#include "robocup_ssl_server.h"
#include <QtNetwork>
#include <iostream>

using namespace std;

RoboCupSSLServer::RoboCupSSLServer(const quint16 & port, const string & net_address, const string & net_interface) :
    _socket(new QUdpSocket()),
    _port(port),
    _net_address(new QHostAddress(QString(net_address.c_str()))),
    _net_interface(new QNetworkInterface(QNetworkInterface::interfaceFromName(QString(net_interface.c_str()))))
{
}


RoboCupSSLServer::RoboCupSSLServer(const quint16 & port, const QHostAddress & addr, const QNetworkInterface & iface) :
    _socket(new QUdpSocket()),
    _port(port),
    _net_address(new QHostAddress(addr)),
    _net_interface(new QNetworkInterface(iface))
{
}


RoboCupSSLServer::~RoboCupSSLServer()
{
    delete _socket;
    delete _net_address;
    delete _net_interface;
}

void RoboCupSSLServer::change_port(const quint16 & port)
{
    _port = port;
}

void RoboCupSSLServer::change_address(const string & net_address)
{
    delete _net_address;
    _net_address = new QHostAddress(QString(net_address.c_str()));
}

void RoboCupSSLServer::change_interface(const string & net_interface)
{
    delete _net_interface;
    _net_interface = new QNetworkInterface(QNetworkInterface::interfaceFromName(QString(net_interface.c_str())));
}

void RoboCupSSLServer::close()
{
    if(_socket->state() == QUdpSocket::BoundState)
        _socket->leaveMulticastGroup(*_net_address, *_net_interface);
}

bool RoboCupSSLServer::open()
{
    close();

    bool success = _socket->bind(_port, QUdpSocket::ShareAddress);
    if(!success) {
        fprintf(stderr,"Unable to open UDP network port: %d\n",_port);
        fflush(stderr);
        return false;
    }

    success = _socket->joinMulticastGroup(*_net_address, *_net_interface);
    if(!success) {
        fprintf(stderr,"Unable to setup UDP multicast\n");
        fflush(stderr);
        return false;
    }

    return true;
}

bool RoboCupSSLServer::send(const SSL_WrapperPacket & packet)
{
    QByteArray datagram;

    datagram.resize(packet.ByteSize());
    bool success = packet.SerializeToArray(datagram.data(), datagram.size());
    if(!success) {
        //TODO: print useful info
        cerr << "Serializing packet to array failed." << endl;
        return false;
    }

    mutex.lock();
    quint64 bytes_sent = _socket->writeDatagram(datagram, *_net_address, _port);
    mutex.unlock();
    if (bytes_sent != datagram.size()) {
        cerr << "Sending UDP datagram failed (maybe too large?). Size was: " << datagram.size() << "byte(s)." << endl;
        return false;
    }

    return true;
}

bool RoboCupSSLServer::send(const SSL_DetectionFrame & frame)
{
    SSL_WrapperPacket pkt;
    SSL_DetectionFrame * nframe = pkt.mutable_detection();
    nframe->CopyFrom(frame);
    return send(pkt);
}

bool RoboCupSSLServer::send(const SSL_GeometryData & geometry)
{
    SSL_WrapperPacket pkt;
    SSL_GeometryData * gdata = pkt.mutable_geometry();
    gdata->CopyFrom(geometry);
    return send(pkt);
}

