// $Id: inode.cpp,v 1.12 2014-07-03 13:29:57-07 - - $

#include <iostream>
#include <stdexcept>

using namespace std;

#include "debug.h"
#include "inode.h"

//declared to be static in header. Has to be declared again in implem.
//file
//initialized to one
int inode::next_inode_nr {1};

//INODE STUFF///////////////////////////////////////////////////////
//inode constructor
inode::inode(inode_t init_type):
   //field initializer list
   //initialized to whatever type of parameter it tends to be
   inode_nr (next_inode_nr++), type (init_type)
{
   switch (type) {
      case PLAIN_INODE:
           contents = make_shared<plain_file>();
           break;
      case DIR_INODE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

size_t inode::size() {
   if (type == PLAIN_INODE) {
      return plain_file_ptr_of(contents)->size();
   }
   return directory_ptr_of(contents)->size();
}

void inode::writefile(const wordvec& data) {
   plain_file_ptr_of(contents)->writefile(data);
}

inode_t inode::get_type() {
   return type;
}

const map<string, inode_ptr>& inode::get_contents(){
   return directory_ptr_of(contents)->get_dirents();
}


//performing cast operations
//conversion methods/////////////////////////////////////////////////
plain_file_ptr plain_file_ptr_of (file_base_ptr ptr) {
                           //function call
   plain_file_ptr pfptr = dynamic_pointer_cast<plain_file> (ptr);
   //check
   if (pfptr == nullptr) throw invalid_argument ("plain_file_ptr_of");
   return pfptr;
}

directory_ptr directory_ptr_of (file_base_ptr ptr) {
   directory_ptr dirptr = dynamic_pointer_cast<directory> (ptr);
   if (dirptr == nullptr) throw invalid_argument ("directory_ptr_of");
   return dirptr;
}

//PLAIN FILE STUFF///////////////////////////////////////////////////
//next few functions have to be updated
size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

string plain_file::readfile() {
   string str = "";
   for (auto elem : data) {
      str = str + elem + " ";
   }
   return str;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   data = words;
}

//DIRECTORY STUFF/////////////////////////////////////////////////////
size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

//directory constructor
directory::directory() {
   dirents.insert({".", nullptr}); 
   dirents.insert({"..", nullptr}); 
}

inode& directory::mkfile(const string& filename) {
   inode_ptr i = make_shared<inode>(PLAIN_INODE);
   if(dirents.find(filename)==dirents.end()) {
      dirents.insert({filename, i});
   }
   return *i;
}

void directory::set_current(inode_ptr dot) {
   dirents.at(".") = dot;
}

void directory::set_parent(inode_ptr dotdot) {
   dirents.at("..") = dotdot;
}

//returns the directory entries since dirents is private
const map<string, inode_ptr>& directory::get_dirents() {
   return dirents;
}
//INODE_STATE STUFF/////////////////////////////////////////////////
inode_state::inode_state() {
   root = make_shared<inode>(DIR_INODE);
   cwd = root;
   directory_ptr_of(root->contents)->set_current(root);
   directory_ptr_of(root->contents)->set_parent(root);
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt << "\"");
}

const map<string, inode_ptr>& inode_state::get_contents() {
   return directory_ptr_of(cwd->contents)->get_dirents();
}

void inode_state::set_cwd(const wordvec pathname, const bool r) {
   //begin at the root directory
   auto i_ptr = cwd;
   if(r) i_ptr = root;
   for(auto it = pathname.begin(); it != pathname.end(); it++) {
      auto p = i_ptr->get_contents().find(*it);
      if (p == i_ptr->get_contents().end()){
         throw yshell_exn("path does not exist");
      }
      i_ptr = p->second;
   }
   cwd = i_ptr;
}

void inode_state::add_file(const wordvec& data, const wordvec& pathname, bool r) {
   //find the desired path!
   auto i_ptr = get_cwd();
   if(r) auto i_ptr = get_root();
   auto it = pathname.begin();
   for (; it != pathname.end()-1; it++) {
      auto p = i_ptr->get_contents().find(*it);
      if (p == i_ptr->get_contents().end()){
         throw yshell_exn("no such path");
      }

   i_ptr = p->second;
   }

   //create the file!
   if(i_ptr->get_contents().find(*it) != i_ptr->get_contents().end()){
      throw yshell_exn("file already exists!");
   }
   else {
      directory_ptr_of(i_ptr->contents)->mkfile(*it).writefile(data);
   }
}

string inode_state::readfile(const wordvec pathname, bool r) {
   auto i_ptr = get_cwd();
   if(r) auto i_ptr = get_root();
   auto it = pathname.begin();
   for(; it!=pathname.end()-1;it++) {
      auto p = i_ptr->get_contents().find(*it);
      if (p == i_ptr->get_contents().end()) {
         throw yshell_exn("no such path");
      }
      i_ptr = p->second;
   }
   auto file = i_ptr->get_contents().find(*(pathname.end()-1));
   if (file != i_ptr->get_contents().end()){
      if(file->second->get_type() == DIR_INODE) {
         throw yshell_exn("not a file");
         return "";
      }
      
      return plain_file_ptr_of(file->second->contents)->readfile();
   }
   else {
      throw yshell_exn("no such file");
      return "";
   }
}

const inode_ptr inode_state::get_root() {
   return root;
}

const inode_ptr inode_state::get_cwd() {
   return cwd;
}

void inode_state::set_prompt(const string& p){
   prompt = p;
}

//OSTREAM OPERATOR/////////////////////////////////////////////////
ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}


