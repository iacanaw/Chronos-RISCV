#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#define MSG_SIZE 256

typedef struct {
    unsigned int length;
    unsigned int msg[ MSG_SIZE ];
} Message;

#endif /* __MESSAGE_H__ */
