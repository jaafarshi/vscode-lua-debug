#include "thunk.h"
#include <memory>
#include <sys/mman.h>
#include <memory.h>

bool thunk::create(size_t s) {
	data = mmap(NULL, s, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (!data) {
		size = 0;
		return false;
	}
	size = s;
	return true;
}

bool thunk::write(void* buf) {
	memcpy(data, buf, size);
	return true;
}

thunk::~thunk() {
	if (!data) return;
	munmap(data, size);
}

thunk* thunk_create_hook(intptr_t dbg, intptr_t hook)
{
	// int __cedel thunk_hook(lua_State* L, lua_Debug* ar)
	// {
	//     `hook`(`dbg`, L, ar);
	//     return `undefinition`;
	// }
	static unsigned char sc[] = {
		0x50,                                                       // push rax
		0x48, 0x83, 0xec, 0x28,                                     // sub rsp, 40
		0x48, 0x89, 0xf2,                                           // mov rdx, rsi
		0x48, 0x89, 0xfe,                                           // mov rsi, rdi
		0x48, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rdi, dbg
		0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, hook
		0xff, 0xd0,                                                 // call rax
		0x48, 0x83, 0xc4, 0x28,                                     // add rsp, 40
		0x58,                                                       // pop rax
		0xc3,                                                       // ret
	};
	std::unique_ptr<thunk> t(new thunk);
	if (!shellcode_create(t.get(), sizeof(sc))) {
		return 0;
	}
	memcpy(sc + 13, &dbg, sizeof(dbg));
	memcpy(sc + 23, &hook, sizeof(hook));
	if (!t->write(&sc)) {
		return 0;
	}
	return t.release();
}

thunk* thunk_create_panic(intptr_t dbg, intptr_t panic)
{
	// int __cedel thunk_panic(lua_State* L)
	// {
	//    `panic`(`dbg`, L);
	//     return `undefinition`;
	// }
	static unsigned char sc[] = {
		0x50,                                                       // push rax
		0x48, 0x83, 0xec, 0x28,                                     // sub rsp, 40
		0x48, 0x89, 0xfe,                                           // mov rsi, rdi
		0x48, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rdi, dbg
		0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, panic
		0xff, 0xd0,                                                 // call rax
		0x48, 0x83, 0xc4, 0x28,                                     // add rsp, 40
		0x58,                                                       // pop rax
		0xc3,                                                       // ret
	};
	std::unique_ptr<thunk> t(new thunk);
	if (!shellcode_create(t.get(), sizeof(sc))) {
		return 0;
	}
	memcpy(sc + 10, &dbg, sizeof(dbg));
	memcpy(sc + 20, &panic, sizeof(panic));
	if (!t->write(&sc)) {
		return 0;
	}
	return t.release();
}

thunk* thunk_create_panic(intptr_t dbg, intptr_t panic, intptr_t old_panic)
{
	if (!old_panic) {
		return thunk_create_panic(dbg, panic);
	}
	// int __cedel thunk_panic(lua_State* L)
	// {
	//    `panic`(`dbg`, L);
	//    `old_panic`(L);
	//     return `undefinition`;
	// }
	static unsigned char sc[] = {
		0x50,                                                       // push rax
		0x57,                                                       // push rdi
		0x48, 0x83, 0xec, 0x28,                                     // sub rsp, 40
		0x48, 0x89, 0xfe,                                           // mov rsi, rdi
		0x48, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rdi, dbg
		0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, panic
		0xff, 0xd0,                                                 // call rax
		0x48, 0x83, 0xc4, 0x28,                                     // add rsp, 40
		0x5f,                                                       // pop rdi
		0x48, 0x83, 0xec, 0x28,                                     // sub rsp, 40
		0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, old_panic
		0xff, 0xd0,                                                 // call rax
		0x48, 0x83, 0xc4, 0x28,                                     // add rsp, 40
		0x58,                                                       // pop rax
		0xc3,                                                       // ret
	};
	std::unique_ptr<thunk> t(new thunk);
	if (!shellcode_create(t.get(), sizeof(sc))) {
		return 0;
	}
	memcpy(sc + 11, &dbg, sizeof(dbg));
	memcpy(sc + 21, &panic, sizeof(panic));
	memcpy(sc + 42, &old_panic, sizeof(old_panic));
	if (!t->write(&sc)) {
		return 0;
	}
	return t.release();
}

