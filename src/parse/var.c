#include <string.h>


void
insert(ll **head, char *val, size_t size) {
        if (*head == NULL) {
            *head = malloc(sizeof(ll));
            *head->val = malloc(size);
            *head->size = size;
            *head->next = NULL;
        } else {
            ll *tmp = *head;
            while (tmp->next) {
                int cmp = strcmp(val, tmp->val);
                if (cmp == 0) {// value already in 
                    return;
                } else if (cmp > 0) {// val > tmp->val -> insert between this and previous pos
                    prev->next = malloc(sizeof(ll));
                    prev->next->size = size;
                    prev->next->val = val;
                } else if (cmp < 0) {// val < tmp->val -> onto next value
                    tmp = tmp->next;
                }
            }
        }
}
