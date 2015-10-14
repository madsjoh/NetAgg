#ifndef LIBNETAGG_H
#define LIBNETAGG_H
#include <linux/types.h>
extern int netagg_open(void);

extern int netagg_close(int fd);

extern int netagg_append_sender(int fd, __be64 vector, __be32 pri_daddr, __be32 sec_daddr, __be16 pri_port, __be16 sec_port);

extern int netagg_append_receiver(int fd, __be32 pri_daddr, __be32 sec_daddr, __be16 pri_port, __be16 sec_port);

extern int netagg_flush_sender(int fd);

extern int netagg_flush_receiver(int fd);


#endif /* LIBNETAGG_H */
