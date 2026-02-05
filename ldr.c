#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define descr_ "LDR\x88"

#define valdesc(ptr) \
!memcmp(ptr, descr_, sizeof(descr_)-1)

// for now, just support
// .text

typedef void (*fp)(void);

// HI\n
u8 text[]=
"\xb8\x48\x49\x0a\x00" // mov eax, 0x0A4948
"\x50"		       // push rax
"\xb8\x01\x00\x00\x00" // mov eax, 1
"\xbf\x01\x00\x00\x00" // mov edi, 1
"\x48\x89\xe6"         // mov rsi, rsp
"\xba\x03\x00\x00\x00" // mov edx, 3
"\x0f\x05"             // syscall
"\xb8\x3c\x00\x00\x00" // mov eax, 60
"\xbf\x00\x00\x00\x00" // mov edi, 0
"\x0f\x05";            // syscall

int texts = sizeof(text)-1; 

typedef struct ldrhdr {
	u8  desc[4];
	u16 ver;
	u16 arch;
	u32 entry;
} ldrhdr;

// have not tested this
int lde(char*dir) {
	int fd = open(dir, O_RDONLY);

	struct stat st;
	if (fstat(fd, &st) < 0) {
		return -1;
	}

	u8*d = mmap(NULL, st.st_size, 
	PROT_READ | PROT_EXEC, MAP_PRIVATE, fd, 0);

	ldrhdr *a = (ldrhdr*)d;

	if (!valdesc(a->desc)) {
		return -1;
	}

	fp entry = (fp)(uintptr_t)a->entry;
	entry();
}

// have not tested this
int ldn(u8*mem) {
	ldrhdr *a = (ldrhdr*)mem;

	if (!valdesc(a->desc)) {
		return -1;
	}

	fp entry = (fp)(uintptr_t)a->entry;
	entry();
}

// have tested this
int main(int argc, char **argv) {
	int e = 1000;
	int s = 2048;

	u8*d = mmap(NULL, s, PROT_READ | PROT_EXEC | PROT_WRITE, 
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	ldrhdr a = {.desc= descr_, .entry = e};
	memcpy(d+e, text, texts);

	fp entry = (fp)(d + a.entry);
    	entry();

	munmap(d,s);
	return 0;
}
