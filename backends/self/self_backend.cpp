#include <cstring>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

extern "C" {
#include "mpi.h"
#include "util.h"
}

class Send {
  public:
    const void *userbuf;
    void *libbuf;
    int count;
    MPI_Datatype datatype;
    int tag; // TODO: handle tag, just change map to <pair<tag, comm>, vector<Send*>>

    Send(const void *userbuf, int count, MPI_Datatype datatype, int tag, MPI_Comm comm)
        : userbuf(userbuf), count(count), datatype(datatype), tag(tag) {
        size_t msg_size = count * nanompi_get_dtype_size(datatype);
        libbuf = malloc(msg_size);
        memcpy(libbuf, userbuf, msg_size);
    }

    ~Send() { free(libbuf); }
};

unordered_map<MPI_Comm, vector<Send *>> map;

extern "C" int nanompi_self_send(const void *buf, int count, MPI_Datatype datatype, int dest,
                                 int tag, MPI_Comm comm) {
    if (dest != comm->my_rank) {
        cout << "error: rank using self send but not to itself!" << endl;
    }

    map[comm].push_back(new Send(buf, count, datatype, tag, comm));

    return MPI_SUCCESS;
}

extern "C" int nanompi_self_recv(void *buf, int count, MPI_Datatype datatype, int source, int tag,
                                 MPI_Comm comm) {
    vector<Send *> &sendq = map[comm];
    vector<Send *>::iterator iter;

    for (iter = sendq.begin(); iter != sendq.end();) {
        if ((*iter)->count <= count && (*iter)->datatype.id == datatype.id) {
            memcpy(buf, (*iter)->libbuf,
                   (*iter)->count * nanompi_get_dtype_size((*iter)->datatype));
            delete *iter;
            sendq.erase(iter);
            return MPI_SUCCESS;
        }
    }

    return MPI_ERR_OTHER;
}
