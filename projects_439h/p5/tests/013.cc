#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"


/*******************************************************************************************************************************/

// these are just some string helpers for me bc we don't have access to full cpp std library :(

// string comparison upto n chars
int cmp_strs(const char* str_1, const char* str_2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (str_1[i] == '\0' || str_2[i] == '\0') { break; }
        else if (str_1[i] != str_2[i]) { return (unsigned char)str_1[i] - (unsigned char)str_2[i]; }
    }
    return 0;
}

// find first occurence of char c in given string (nullptr if not)
char* first_occur(char* str, char c) {
    while (*str != '\0') {
        if (*str == c) { return str; }
        else { str++; }   
    }
    return nullptr; 
}

/*******************************************************************************************************************************/

void simulate_script_execution(StrongPtr<Node> script_file, StrongPtr<Node> script_directory, StrongPtr<Ext2> fs) {
    if (script_file==nullptr || (script_file->is_file()==false)) {
        Debug::printf("*** uh-oh, did not find the file\n");
        return;
    }

    Debug::printf("*** simulating execution of shell script...\n");

    auto size_bytes = script_file->size_in_bytes();
    auto buffer = new char[size_bytes + 1];
    auto bytes_read = script_file->read_all(0, size_bytes, buffer);
    buffer[size_bytes] = 0;

    if (bytes_read < size_bytes) {
        Debug::printf("*** uh-oh, did not read all the bytes\n");
        delete[] buffer;
        return;
    }

    Debug::printf("*** check the contents:\n");
    // for your debugging purposes:
    // for (uint32_t i=0; i<bytes_read; i++) {
    //     Debug::printf("%c",buffer[i]);
    // }

    char* line = buffer;
    while (*line != '\0') {
        if (cmp_strs(line, "ls | wc -l", 10) == 0) {
            if (script_directory->is_dir()) {
                uint32_t file_count = script_directory->entry_count();
                Debug::printf("*** The scripts/ directory contains %d files.\n", file_count);
            } else {
                Debug::printf("*** The scripts/ directory could not be found or is not a directory.\n");
            }
        } else if (cmp_strs(line, "cat a.txt", 9) == 0) {
            auto file_a = fs->find(script_directory, "a.txt");
            if (file_a->is_file()) {
                Debug::printf("*** cat a.txt: ");
                auto file_size = file_a->size_in_bytes();
                char* file_buffer = new char[file_size + 1];
                file_a->read_all(0, file_size, file_buffer);
                file_buffer[file_size] = 0;
                Debug::printf("%s\n", file_buffer);
                delete[] file_buffer;
            } else {
                Debug::printf("*** a.txt not found or not a file.\n");
            }
        } else if (cmp_strs(line, "file b.txt", 10) == 0) {
            auto file_a = fs->find(script_directory, "b.txt");
            if (file_a->is_file()) {
                Debug::printf("*** b.txt is a regular file.\n");
            } else if (file_a->is_dir()) {
                Debug::printf("*** b.txt is a directory (unexpected).\n");
            } else if (file_a->is_symlink()) {
                Debug::printf("*** b.txt is a symbolic link (unexpected).\n");
            } else {
                Debug::printf("*** b.txt not found (unexpected).\n");
            }
        } else {
            Debug::printf("*** Unrecognized command: %s\n", line);
        }

        line = first_occur(line, '\n');
        if (line == nullptr) break;
        line++;
    }

    delete[] buffer;
}

void test_empty_directory(StrongPtr<Node> empty_dir) {
    if (empty_dir == nullptr || !empty_dir->is_dir()) {
        Debug::printf("*** empty_dir could not be found or is not a directory.\n");
        return;
    }

    uint32_t entry_count = empty_dir->entry_count();
    Debug::printf("*** entry count in empty_dir: %d\n", entry_count);


    if (entry_count == 3) { 
        Debug::printf("*** empty_dir is 'empty' as expected (. and .. exist)\n");
    } else {
        Debug::printf("*** unexpected entry count in empty_dir\n");
    }
}

/* Called by one core */
void kernelMain(void) {
    auto ide = StrongPtr<Ide>::make(1, 3);
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** Block size: %d\n", fs->get_block_size());
    Debug::printf("*** Inode size: %d\n", fs->get_inode_size());

    // find scripts/ directory
    auto script_dir = fs->find(fs->root, "scripts");
    if (script_dir == nullptr || !script_dir->is_dir()) {
        Debug::printf("*** The scripts/ directory was not found or is not a directory\n");
        return;
    }

    // grab script.sh and "run" it
    auto script_file = fs->find(script_dir, "script.sh");
    simulate_script_execution(script_file, script_dir, fs);

    //test empty directory edge case
    auto empty_dir = fs->find(fs->root, "empty_dir");
    test_empty_directory(empty_dir);

}
