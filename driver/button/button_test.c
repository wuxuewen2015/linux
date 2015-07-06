#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
int main(void)
{
 int buttons_fd;
 char buttons[6] = {'0', '0', '0', '0', '0', '0'};
 buttons_fd = open("/dev/button_dev",0);
 if(buttons_fd<0){
  perror("open device buttons");
  exit(1);
  }
   for(;;){
   	       char current_buttons[6];
           fd_set rds;
           int ret,i,count_of_changed_key;
           FD_ZERO(&rds);
           FD_SET(buttons_fd,&rds);
           ret = select(buttons_fd+1,&rds,NULL,NULL,NULL);
           if(ret<0)
            {
              perror("select");
             exit(1);
            }
            if(ret==0){printf("timeout\n");}
             else if (FD_ISSET(buttons_fd,&rds))
             {
              int ret=read(buttons_fd, current_buttons, sizeof(current_buttons)) ;
              for (i = 0, count_of_changed_key = 0; i < sizeof(buttons) /sizeof(buttons[0]); i++) {
			   if (buttons[i] != current_buttons[i]) {
				buttons[i] = current_buttons[i];
				printf("%skey %d is %s", count_of_changed_key? ", ": "", i+1, buttons[i] == '0' ? "up" : "down");
				count_of_changed_key++;
			}
		}
		if (count_of_changed_key) {
			printf("\n");
		}

  }
             close(buttons_fd);
             return 0;
}
}
