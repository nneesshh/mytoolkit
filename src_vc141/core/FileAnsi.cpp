#include "FileAnsi.h"

bool
CRaFile::Open( const char *filename, int mode, bool bin ) {
	char chMode[25] = { 0 };

	if (!strlen(filename))
		return false;

	Close();

	m_iswritable = false;

	if (mode & modeCreate) {
		if (mode & modeNoTruncate) {
			strcat(chMode,"a+");
		}
		else {
			strcat(chMode,"w+");
		}
		m_iswritable = true;
	}
	else if (mode & modeReadWrite) {
		strcat(chMode,"w+");
		m_iswritable = true;
	}
	else if (mode & modeRead) {
		strcat(chMode,"r");
	}
	else if (mode & modeWrite) {
		strcat(chMode,"w");
		m_iswritable = true;
	}

	if (bin)
		strcat(chMode,"b");

	m_file = fopen(filename, chMode);
	if (m_file) {
		m_pathname = filename;
		if (!(mode & modeNoTruncate)) {
			Seek(0,BEGIN);
		}

// #ifdef _WIN32
// 		_setmode(_fileno(m_file),_O_BINARY);
// #endif
	}

	return (nullptr != m_file);
}

/* -- EOF -- */