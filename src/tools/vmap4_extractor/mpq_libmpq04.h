/*
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MPQ_H
#define MPQ_H

#include "loadlib/loadlib.h"
#include "libmpq/mpq.h"
#include <string.h>
#include <ctype.h>
#include <vector>
#include <iostream>
#include <deque>

using namespace std;

class MPQArchive
{

public:
    mpq_archive_s *mpq_a;

    MPQArchive(const char* filename);
    ~MPQArchive() { if (isOpened()) close(); }

    void GetFileListTo(vector<string>& filelist) {
        uint32_t filenum;
        if(libmpq__file_number(mpq_a, "(listfile)", &filenum)) return;
        libmpq__off_t size, transferred;
        libmpq__file_size_unpacked(mpq_a, filenum, &size);

        char *buffer = new char[size + 1];
        buffer[size] = '\0';

        libmpq__file_read(mpq_a, filenum, (unsigned char*)buffer, size, &transferred);

        char seps[] = "\n";
        char *token;

        token = strtok( buffer, seps );
        libmpq__off_t counter = 0;
        while ((token != NULL) && (counter < size)) {
            //cout << token << endl;
            token[strlen(token) - 1] = 0;
            string s = token;
            filelist.push_back(s);
            counter += strlen(token) + 2;
            token = strtok(NULL, seps);
        }

        delete[] buffer;
    }

private:
    void close();private:
    bool isOpened() const;
};
typedef std::deque<MPQArchive*> ArchiveSet;

class MPQFile
{
    //MPQHANDLE handle;
    bool eof;
    char *buffer;
    libmpq__off_t pointer,size;

    // disable copying
    MPQFile(const MPQFile& /*f*/) {}
    void operator=(const MPQFile& /*f*/) {}

public:
    MPQFile(const char* filename);    // filenames are not case sensitive
    ~MPQFile() { close(); }
    size_t read(void* dest, size_t bytes);
    size_t getSize() { return size; }
    size_t getPos() { return pointer; }
    char* getBuffer() { return buffer; }
    char* getPointer() { return buffer + pointer; }
    bool isEof() { return eof; }
    void seek(size_t offset);
    void seekRelative(size_t offset);
    void close();
};

inline void flipcc(char *fcc)
{
    std::swap(fcc[0], fcc[3]);
    std::swap(fcc[1], fcc[2]);
}

#endif
