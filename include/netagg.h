#ifndef LIBNETAGG_H
#define LIBNETAGG_H
#include <linux/types.h>
extern int netagg_open(void);

extern int netagg_close(int fd);

extern int netagg_append_sender(int fd, struct rule *rule);

extern int netagg_append_receiver(int fd, struct rule *rule);

extern int netagg_flush_sender(int fd);

extern int netagg_flush_receiver(int fd);


#endif /* LIBNETAGG_H */
