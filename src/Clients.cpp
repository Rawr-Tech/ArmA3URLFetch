
#include "Clients.h"

bool Clients::GetClient(int id, Clients::Client *client)
{
    if (clients.find(id) == clients.end())
        return false;
    
    clientsMtx.lock();
    *client = clients[id];
    clientsMtx.unlock();

    return true;
};

void Clients::setClient(int id, Clients::Client cli)
{
    clientsMtx.lock();
    if (clients.find(id) != clients.end())
        clients[id] = cli;
    clientsMtx.unlock();
};

bool Clients::setParameters(int id, std::map<std::string, std::string> params)
{
    Clients::Client client;
    if (!GetClient(id, &client))
        return false;

    for (std::map<std::string, std::string>::iterator it = params.begin(); it != params.end(); ++it)
        client.Parameters[it->first] = it->second;
    
    setClient(id, client);

    return true;
};

bool Clients::setHeaders(int id, std::vector<std::string> headers)
{
    Clients::Client client;
    if (!GetClient(id, &client))
        return false;
    
    client.Headers = headers;
    setClient(id, client);

    return true;
};

int Clients::addClient(std::map<std::string, std::string> params)
{
    Clients::Client client;
    client.Parameters = params;
    
    int key = 1;
    while (true)
    {
        if (clients.find(key) == clients.end())
            break;
        key++;
    }

    clientsMtx.lock();
    clients[key] = client;
    clientsMtx.unlock();

    return key;
};

int Clients::AddClient(Output *op, std::map<std::string, std::string> params)
{
    int id = addClient(params);
    if (id <= 0)
        return 101;
    
    op->Write(id);
    return 100;
};

int Clients::RemoveClient(Output *op, int id)
{
    op->Write(id);
    if (!removeClient(id))
        return 201;
    
    return 200;
};

int Clients::SetParameters(Output *op, int id, std::map<std::string, std::string> params)
{
    op->Write(id);
    if (!setParameters(id, params))
        return 301;
    
    return 300;
};

int Clients::SetHeaders(Output *op, int id, std::vector<std::string> headers)
{
    op->Write(id);
    if (!setHeaders(id, headers))
        return 401;
    
    return 400;
};

bool Clients::removeClient(int id)
{
    std::map<int, Clients::Client>::iterator f;
    f = clients.find(id);

    if (f == clients.end())
        return false;
    
    clientsMtx.lock();
    clients.erase(f);
    clientsMtx.unlock();

    return true;
};
