#include <debugger/io/namedpipe.h>
#include <debugger/client/run.h>
#include <debugger/io/base.h>
#include <debugger/io/helper.h>
#include <debugger/client/stdinput.h>
#include <base/win/process_switch.h>

static void sleep() {
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

bool run_pipe_attach(stdinput& io, vscode::rprotocol& init, vscode::rprotocol& req, const std::wstring& pipename, base::win::process_switch* m)
{
	vscode::io::namedpipe pipe;
	if (!pipe.open_client(pipename, 60000)) {
		return false;
	}
	io_output(&pipe, init);
	io_output(&pipe, req);
	for (; !pipe.is_closed(); sleep()) {
		io.update(10);
		pipe.update(10);
		std::string buf;
		while (io.input(buf)) {
			pipe.output(buf.data(), buf.size());
		}
		while (pipe.input(buf)) {
			io.output(buf.data(), buf.size());
			if (m) {
				m->unlock();
				m = nullptr;
			}
		}
	}
	return true;
}
