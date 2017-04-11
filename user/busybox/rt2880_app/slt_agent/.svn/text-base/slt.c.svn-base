#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>  
#include <time.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/select.h>

#include <linux/autoconf.h>

#define SLT_AGENT_VERSION	"1.2"

#define HANDLER_UART		"/dev/ttyS1"
#define BIN_RESULT		"/proc/slt/bin"
#define WIFI_RESULT		"/proc/slt/wifi"
#define SLT_DUT_TEST_TIME	90

void usage(void)
{
        printf("Usage: slt ROLE [PLATFORM]\n");
        printf("       ROLE - \"dut\", \"link_partner\"\n");
        printf("       PLATFORM- \"RT6856\", \"MT7620\" ...\n");
}

int slt_dut(char *platform)
{
	time_t t;
	int fd, fd2;
	size_t rc;
	unsigned char buf[128];
	int i, count;
	int test_time;
	int bin_result, wifi_result;

	system("stty -F /dev/ttyS1 -echo");
	system("stty -F /dev/ttyS1 cbreak");

	printf("SLT Agent (v%s): start\n", SLT_AGENT_VERSION);

	fd = open(HANDLER_UART, O_RDWR | O_NONBLOCK);
	if (fd == -1)
		perror("open:");

#if 0
	i = 0;
	count = 0;
	memset(buf, 0, sizeof(buf));
	printf("SLT Agent: Wait for command \"START\" from handler\n");
	t = time(NULL);
	while (1) {
		int ret ;

		if (time(NULL) != t) {
			t = time(NULL);
			count++;
			if (count >= 120) {
				printf("SLT Agent: Wait command \"START\" timeout!\n");
				close(fd);
				return 0;
			}
			if ((count % 10) == 0) 
				printf("SLT Agent: Wait for command \"START\" from handler\n");
		}

		rc = read(fd, buf + i, 1);
		i += (rc == -1 ? 0 : rc);
		if (strstr(buf, "#START%")) {
			printf("SLT Agent: Get \"START\" from handler\n");
			break;
		}
		
		if (i == (sizeof(buf) - 1))
			i = 0;
	}
#endif

	i = 0;
	count = 0;
	memset(buf, 0, sizeof(buf));
	printf("\n\nSLT Agent: send command \"DOSOK\" to handler\n");
	t = time(NULL);
	while (0) {
		int ret ;

		if ((time(NULL) != t) || (count == 0)) {
			t = time(NULL);
			ret = write(fd, "DOSOK\n", strlen("DOSOK\n"));
			if (ret != strlen("DOSOK\n"))
				printf("SLT Agent: send command \"DOSOK\" failed\n");
		
			count++;
			if (count >= 120) {
				printf("SLT Agent: Get \"NEXT\" timeout!\n");
				close(fd);
				return 0;
			}
			if ((count % 10) == 0) 
				printf("SLT Agent: Wait for command \"NEXT\" from handler\n");
		}

		rc = read(fd, buf + i, 1);
		i += (rc == -1 ? 0 : rc);
		if (strstr(buf, "#NEXT%")) {
			printf("SLT Agent: Get \"NEXT\" from handler\n");
			break;
		}
		
		if (i == (sizeof(buf) - 1))
			i = 0;
	}

	printf("SLT Agent: Start DUT test\n");
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "slt_test.sh %s &", platform);
	system(buf);

#if defined (CONFIG_RALINK_MT7620)
	test_time = SLT_DUT_TEST_TIME;
	while (test_time) {
		fd2 = open(BIN_RESULT, O_RDONLY);
		if (fd2 == -1) {
			perror("open:");
			sleep(1);
			continue;
		}

		memset(buf, 0, sizeof(buf));
		read(fd2, buf, sizeof(buf));
		close(fd2);
		bin_result = atoi(buf);
		if (bin_result != 0)
			break;

		sleep(1);
		test_time--;
	}

	if (bin_result == 1) {
		while (test_time) {
			fd2 = open(WIFI_RESULT, O_RDONLY);
			if (fd2 == -1) {
				perror("open:");
				sleep(1);
				continue;
			}

			memset(buf, 0, sizeof(buf));
			read(fd2, buf, sizeof(buf));
			close(fd2);
			wifi_result = atoi(buf);
			if (wifi_result != 0) {
				bin_result = wifi_result;
				break;
			}

			sleep(1);
			test_time--;
		}
	}

	test_time = SLT_DUT_TEST_TIME;
	if (bin_result == 1) {	
		while (test_time) {
			int i;

			rc = write(fd, "ERR PASS0000 00000000 00000000\n", strlen("ERR PASS0000 00000000 00000000\n"));
			if (rc != strlen("ERR PASS0000 00000000 00000000\n"))
				printf("SLT Agent: send result failed\n");

			sleep(1);
			test_time--;
		}
	}
	else {
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "ERR FAIL000%d 00000000 00000000\n", bin_result);
		while (test_time) {
			rc = write(fd, buf, strlen(buf));
			if (rc != strlen(buf))
				printf("SLT Agent: send result failed\n");

			sleep(1);
			test_time--;
		}
	}
	close(fd);
#endif


	return 0;
}


int slt_link_partner(char *platform)
{
	char buf[128];

	printf("SLT Link Partner (v%s): Platrom %s\n", SLT_AGENT_VERSION, platform);
	sprintf(buf, "slt_link_partner.sh %s", platform);
	system(buf);
	
	return 0;
}



int main(int argc, char *argv[])
{
	if (argc != 3) {
		usage();
		return 0;
	}
	else {
		if (strcmp(argv[1], "dut") == 0)
			slt_dut(argv[2]);
		else if (strcmp(argv[1], "link_partner") == 0)
			slt_link_partner(argv[2]);
		else {
			usage();
			return 0;
		}
	}


	return 0;
}

