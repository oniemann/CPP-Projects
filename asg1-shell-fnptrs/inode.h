// $Id: inode.h,v 1.13 2014-06-12 18:10:25-07 - - $

#ifndef __INODE_H__
#define __INODE_H__

#include <exception>
#include <iostream>
#include <memory>
#include <map>
#include <vector>
using namespace std;

#include "util.h"

//
// inode_t -
//    An inode is either a directory or a plain file.
//

enum inode_t {PLAIN_INODE, DIR_INODE};
class inode;
class file_base;
class plain_file;
class directory;
using inode_ptr = shared_ptr<inode>;
using file_base_ptr = shared_ptr<file_base>;
using plain_file_ptr = shared_ptr<plain_file>;
using directory_ptr = shared_ptr<directory>;

//
// inode_state -
//    A small convenient class to maintain the state of the simulated
//    process:  the root (/), the current directory (.), and the
//    prompt.
//

class inode_state {
   friend class inode;
   friend ostream& operator<< (ostream& out, const inode_state&);
   private:
      inode_state (const inode_state&) = delete; // copy ctor
      inode_state& operator= (const inode_state&) = delete; // op=
      inode_ptr root {nullptr};
      inode_ptr cwd {nullptr};
      string prompt {"% "};
   public:
      inode_state();
      const map<string, inode_ptr>& get_contents();
      const inode_ptr get_cwd();
      const inode_ptr get_root();
      const string& get_prompt();
      void set_curr(inode_ptr dot);
      void set_parent(inode_ptr dotdot);
      const map<string, inode_ptr>& get_dirents();
      void set_cwd(const wordvec pathname, const bool r);
      void set_prompt(const string& p);
      void add_directory(const wordvec pathname, const bool r);
      void add_file(const wordvec& data,const wordvec& pathname,bool r);
      string readfile(const wordvec pathname, bool r);
;
};


//
// class inode -
//
// inode ctor -
//    Create a new inode of the given type.
// get_inode_nr -
//    Retrieves the serial number of the inode.  Inode numbers are
//    allocated in sequence by small integer.
// size -
//    Returns the size of an inode.  For a directory, this is the
//    number of dirents.  For a text file, the number of characters
//    when printed (the sum of the lengths of each word, plus the
//    number of words.
//    

class inode {
   friend class inode_state;
   private:
      //static removes this from class
      //within class for visibility purposes (nothing else)
      static int next_inode_nr; //int is size_t
      int inode_nr;
      inode_t type;
      file_base_ptr contents;
   public:
      inode (inode_t init_type);
      //Const refers to how an object is actually modified
      //returns specific object's inode number
      //Different than "static" method b/c it still varies between
      //objects
      size_t size();
      int get_inode_nr() const;
      const map<string, inode_ptr>& get_contents();
      void writefile(const wordvec& data);
      inode_t get_type();
};

//
// class file_base -
//
// Just a base class at which an inode can point.  No data or
// functions.  Makes the synthesized members useable only from
// the derived classes.
//


class file_base {
   protected:
      //a bunch of redundant files
      file_base () = default;
      file_base (const file_base&) = default;
      file_base (file_base&&) = default;
      file_base& operator= (const file_base&) = default;
      file_base& operator= (file_base&&) = default;
      virtual ~file_base () = default;
      //this is an abstract function
         //like an interface in java (has to be overwritten by something
         //else)
      virtual size_t size() const = 0;
   public:
      //converts file_base_ptr into a plainfileptr
      friend plain_file_ptr plain_file_ptr_of (file_base_ptr);
      //"                            " directoryptr
      friend directory_ptr directory_ptr_of (file_base_ptr);
};


//
// class plain_file -
//
// Used to hold data.
// synthesized default ctor -
//    Default vector<string> is a an empty vector.
// readfile -
//    Returns a copy of the contents of the wordvec in the file.
//    Throws an yshell_exn for a directory.
// writefile -
//    Replaces the contents of a file with new contents.
//    Throws an yshell_exn for a directory.
//

class plain_file: public file_base {
   private:
      //just a wordvec
      //=vector<string>
      wordvec data;
   public:
      size_t size() const override;
      //reads the file
      string readfile();
      //takes a vector and puts it into a wordvec
      void writefile (const wordvec& newdata);
};

//
// class directory -
//
// Used to map filenames onto inode pointers.
// default ctor -
//    Creates a new map with keys "." and "..".
// remove -
//    Removes the file or subdirectory from the current inode.
//    Throws an yshell_exn if this is not a directory, the file
//    does not exist, or the subdirectory is not empty.
//    Here empty means the only entries are dot (.) and dotdot (..).
// mkdir -
//    Creates a new directory under the current directory and 
//    immediately adds the directories dot (.) and dotdot (..) to it.
//    Note that the parent (..) of / is / itself.  It is an error
//    if the entry already exists.
// mkfile -
//    Create a new empty text file with the given name.  Error if
//    a dirent with that name exists.

class directory: public file_base {
   private:
      //map from a string onto an inode ptr
      map<string,inode_ptr> dirents;
   public:
      //override meaning: takes over how it was originally defined
      size_t size() const override;
      //removes directory
      void remove (const string& filename);
      //makes directory
      inode& mkdir (const string& dirname);
      //makes a file
      inode& mkfile (const string& filename);
      void set_current(inode_ptr dot);
      void set_parent(inode_ptr dotdot);
      const map<string, inode_ptr>& get_dirents();
      directory();
};

#endif

