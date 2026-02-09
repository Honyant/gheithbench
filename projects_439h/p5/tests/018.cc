#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"

//THOROUGH FUNCTIONALITY TESTING
//This test case simulates a very slow disk and checks all basic functionality such as:
//Finding files in root directory
//Finding files within folders
//Nonexistant file sanity check
//Finding symlinks
//Finding files using symlinks
//Finding files that are connected with 5+ chains of symlinks
//Caching of reads + finds

void show(const char* name, StrongPtr<Node> node, bool show) {

    Debug::printf("*** You are currently at %s :D\n",name);

    if (node == nullptr) {
        Debug::printf("***      I do not exist D:\n");
        return;
    } 

    if (node->is_dir()) {
        Debug::printf("***      I'm a directory!!\n");
        Debug::printf("***      I have %d entries and %d links\n",node->entry_count(),node->n_links());
    } else if (node->is_symlink()) {
        auto sz = node->size_in_bytes();
        Debug::printf("***      I'm a symbolic link and my link size is %d\n", sz);
        auto buffer = new char[sz+1];
        buffer[sz] = 0;
        node->get_symbol(buffer);
        Debug::printf("***       => %s\n",buffer);
    } else if (node->is_file()) {
        Debug::printf("***      I'm a file!! \n");
        auto sz = node->size_in_bytes();
        Debug::printf("***      I contain %d bytes and have %d link(s)\n",sz, node->n_links());
        if (show) {
            auto buffer = new char[sz+1];
            buffer[sz] = 0;
            auto cnt = node->read_all(0,sz,buffer);
            CHECK(sz == cnt);
            CHECK(K::strlen(buffer) == cnt);
            for (uint32_t i=0; i<cnt; i++) {
                Debug::printf("%c",buffer[i]);
            }
            delete[] buffer;
            Debug::printf("\n");
        }
    } else {
        Debug::printf("***    I am not the type I expect to be. I am a %d\n",node->get_type());
    }
}

/* Called by one core */
void kernelMain(void) {

    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 50); //A really slow disk
    
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** My system's block size is %d\n",fs->get_block_size());
    Debug::printf("*** The inode size is %d\n",fs->get_inode_size());
   
   // PART 0: get "/"
   auto root = fs->root;
   show("/",root,true);

   // PART 1: Basic file find from root
   auto mermaid = fs->find(root,"mermaid.txt");
   CHECK(root->number != mermaid->number);
   show("/mermaid.txt",mermaid,true);

   // PART 2: File find in a folder
   auto htwoo = fs->find(root,"h2o");
   show("/h2o",htwoo,true);

   auto welcome = fs->find(htwoo,"welcome.txt");
   show("/welcome.txt",welcome,true);

   // PART 3: A character, oops I mean a file, that should be nonexistent
   auto charlotte = fs->find(root,"charlotte.txt");
   show("/charlotte.txt",charlotte,true);

   // PART 4: Follow a symlink!
   auto emma = fs->find(root,"cryokenesis");
   show("/cryokenesis", emma, true);

   // PART 5: Find a file using a symlink connection
    auto emmaquotes = fs->find(root,"cryokenesis/quotes.txt");
    show("/cryokenesis/quotes.txt", emmaquotes, false);
    {
       Debug::printf("***      ");
       uint32_t start = 288;
       uint32_t count = 90;
       for (uint32_t i = 0; i<count; i++) {
           char c;
           emmaquotes->read(start+i,c);
           Debug::printf("%c",c);
       }
       Debug::printf("\n");
   }

   // PART 6a: Symlink leading to a symlink leading to a file
   auto rikki = fs->find(root, "relationships/quotes.txt");
   show("/relationships/quotes.txt, which is what Rikki used to think of Zane", rikki, true);

   // PART 6b: Symlink leading to a symlink which has a symlink inside which leads to a subfolder with a file in it
   auto zane = fs->find(root, "relationships/akward/lore.txt");
   show("relationships/akward/lore.txt, which is the lore between Rikki and Zane", zane, true);

   // PART 7: Following a symlink to a symlink which weaves in and out through folders, checks for correct connection
   auto cleo = fs->find(htwoo,"NAUR");

   //Note: NAUR is a symbolic link to hydrokenesis which is a symbolic link to cleo which has the file quotes.txt
   show("a symbolic link to another symlink of Cleo's folder, /hydrokenesis!", cleo, true);

   // PART 8: Basic read data caching, ideally using some kind of most recently used cache
   auto lewis = fs->find(root, "hydrokenesis/quotes.txt");
   show("hydrokenesis/quotes.txt", lewis, false);
      {
       uint32_t start = 0;
       uint32_t count = 250;
       for (uint32_t k = 0; k < 200; ++k) {
        for (uint32_t i = 0; i<count; i++) {
           char c;
           lewis->read(start+i,c);
           if (k == 0) Debug::printf("%c",c);
       }

       }
       
       Debug::printf("\n");
   }

   // PART 9: Randomly finding things that have been found before to test cache
   auto mermaid1 = fs->find(root,"mermaid.txt");
   auto htwoo1 = fs->find(root,"h2o");
   auto welcome1 = fs->find(htwoo,"welcome.txt");
   auto charlotte1 = fs->find(root,"charlotte.txt");
   auto emma1 = fs->find(root,"cryokenesis");
   auto emmaquotes1 = fs->find(root,"cryokenesis/quotes.txt");
   auto rikki1 = fs->find(root, "relationships/quotes.txt");
   auto zane1 = fs->find(root, "relationships/akward/lore.txt");
   auto cleo1 = fs->find(htwoo,"NAUR");
   auto lewis1 = fs->find(root, "hydrokenesis/quotes.txt");

   Debug::printf("*** Cleo Sertori: Naurrrr!! The test case is over! Good job for passing it tho :D \n");
   Debug::printf("*** Cleo Sertori: The moon spell...\n");

   // PART 10: Chain of 13 symlinks to find a file
   auto moon = fs->find(root, "moon-spell/youareunderthemoonspell/moon.txt");
   show("sea, under the moon spell", moon, true);

}

