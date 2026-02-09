#include "ext2.h"
#include "ide.h"
#include "kernel.h"
#include "libk.h"

// This test case thoroughly tests find().

// This is the first case to test traversal of up to 16 symbolic links.

// This test case also uses block size of 2048 for variety.

void show(const char *name, StrongPtr<Node> node, bool show);

void kernelMain(void) {
  auto ide = StrongPtr<Ide>::make(1, 3);
  auto fs = StrongPtr<Ext2>::make(ide);

  Debug::printf("*** block size is %d\n", fs->get_block_size());
  Debug::printf("*** inode size is %d\n", fs->get_inode_size());

  Debug::printf("*** TESTING BASIC PATH TRAVERSAL\n");

  Debug::printf("*** Test: Find Root\n");
  auto root = fs->root;
  show("/", root, true);

  Debug::printf("*** Test: Find Child of Root\n");
  auto child = fs->find(root, "root_child");
  show("/root_child", child, true);

  Debug::printf(
      "*** Test: Find Nested Child from Root and from Child of Root\n");
  auto nested1 = fs->find(root, "root_child/nested_child");
  show("/root_child/nested_child", nested1, true);
  auto nested2 = fs->find(child, "nested_child");
  show("/root_child/nested_child", nested2, true);
  CHECK(nested1->number == nested2->number);

  Debug::printf("*** TESTING SYMBOLIC LINKS\n");

  Debug::printf("*** Test: Symbolic Link - Relative Path\n");
  auto relative = fs->find(root, "relative");
  show("/relative", relative, true);

  Debug::printf("*** Test: Symbolic Link - Absolute Path\n");
  auto absolute = fs->find(root, "link_dir/absolute");
  show("/link_dir/absolute", absolute, true);

  Debug::printf(
      "*** Test: Resolving Symbolic Link Ending With Slash - Relative Path\n");
  auto resolved_relative = fs->find(root, "relative/");
  CHECK(resolved_relative->number != root->number);
  CHECK(resolved_relative->number == nested1->number);
  CHECK(resolved_relative->number == nested2->number);
  show("/relative/", resolved_relative, true);

  Debug::printf(
      "*** Test: Resolving Symbolic Link Ending With Slash - Absolute Path\n");
  auto resolved_absolute = fs->find(root, "link_dir/absolute/");
  CHECK(resolved_absolute->number != root->number);
  CHECK(resolved_absolute->number == nested1->number);
  CHECK(resolved_absolute->number == nested2->number);
  show("/link_dir/absolute/", resolved_absolute, true);

  Debug::printf("*** Test: Symbolic Link to Root\n");
  auto rootlink = fs->find(root, "link_to_root");
  show("/link_to_root", rootlink, true);
  auto resolve_rootlink = fs->find(root, "link_to_root/");
  show("/link_to_root/", resolve_rootlink, true);
  CHECK(resolve_rootlink->number == root->number);

  Debug::printf("*** Test: Symbolic Link to Non-Existent Path\n");
  auto invalid = fs->find(root, "link_to_invalid");
  show("/link_to_invalid", invalid, true);

  // If you timeout here and your find() is recursive, you may be stack
  // overflowing.
  Debug::printf("*** Test: Following At Least 16 Symbolic Links \n");
  auto links = fs->find(root, "link1");
  show("/link1", links, true);
  auto resolved_links = fs->find(root, "link1/");
  show("/link1/", resolved_links, true);

  auto file = fs->find(resolved_links, "end_of_link_traversal.txt");
  show("/link_dir/end_of_link_traversal.txt", file, true);

  Debug::printf("TESTING . and .. TRAVERSALS\n");

  Debug::printf("*** Test: Path with . \n");
  auto path_with_dot = fs->find(root, "/././././././././././link1");
  show("/././././././././././link1", path_with_dot, true);
  auto resolved_path_with_dot = fs->find(root, "/././././././././././link1/");
  show("/././././././././././link1/", resolved_path_with_dot, true);

  Debug::printf("*** Test: Path with .. \n");
  auto path_with_dot_dot =
      fs->find(root, "root_child/../root_child/../root_child/nested_child");
  show("/root_child/../root_child/../root_child/nested_child",
       path_with_dot_dot, true);
  auto resolved_path_with_dot_dot =
      fs->find(root, "root_child/../root_child/../root_child/nested_child/");
  show("/root_child/../root_child/../root_child/nested_child/",
       resolved_path_with_dot_dot, true);
  auto dot_child = fs->find(resolved_path_with_dot_dot, "file");
  show("/root_child/nested_child/file", dot_child, true);

  Debug::printf("*** Test: Symbolic Link with .. \n");
  auto dotlink = fs->find(root, "dotlink");
  show("/dotlink", dotlink, true);
  auto resolved_dotlink = fs->find(root, "dotlink/");
  show("/dotlink/", resolved_dotlink, true);
}

void show(const char *name, StrongPtr<Node> node, bool show) {

  Debug::printf("*** looking at %s\n", name);

  if (node == nullptr) {
    Debug::printf("***      does not exist\n");
    return;
  }

  if (node->is_dir()) {
    Debug::printf("***      is a directory\n");
    Debug::printf("***      contains %d entries\n", node->entry_count());
    Debug::printf("***      has %d links\n", node->n_links());
  } else if (node->is_symlink()) {
    Debug::printf("***      is a symbolic link\n");
    auto sz = node->size_in_bytes();
    Debug::printf("***      link size is %d\n", sz);
    auto buffer = new char[sz + 1];
    buffer[sz] = 0;
    node->get_symbol(buffer);
    Debug::printf("***       => %s\n", buffer);
  } else if (node->is_file()) {
    Debug::printf("***      is a file\n");
    auto sz = node->size_in_bytes();
    Debug::printf("***      contains %d bytes\n", sz);
    Debug::printf("***      has %d links\n", node->n_links());
    if (show) {
      auto buffer = new char[sz + 1];
      buffer[sz] = 0;
      auto cnt = node->read_all(0, sz, buffer);
      CHECK(sz == cnt);
      CHECK(K::strlen(buffer) == cnt);
      // can't just print the string because there is a 1000 character limit
      // on the output string length.
      for (uint32_t i = 0; i < cnt; i++) {
        Debug::printf("%c", buffer[i]);
      }
      delete[] buffer;
      Debug::printf("\n");
    }
  } else {
    Debug::printf("***    is of type %d\n", node->get_type());
  }
}
