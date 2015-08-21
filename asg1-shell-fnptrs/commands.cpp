// $Id: commands.cpp,v 1.11 2014-06-11 13:49:31-07 - - $

#include "commands.h"
#include "debug.h"

//initializes the commands map
//set of pairs
commands::commands(): map ({
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
}){}

//do not use a subscript operator on a map
//e.g. m[k] and m is a map
//  ----> searches the map from the key k
//  ----> if does not find it, returns the default value
//  ----> will pollute map with a bunch of default values
command_fn commands::at (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   //can use "auto" instead of command_map::const_iterator
   command_map::const_iterator result = map.find (cmd);
   //loops through until it hits the end
   if (result == map.end()) {
      throw yshell_exn (cmd + ": no such function");
   }
   return result->second;
}

//ALL THE STUBS
void fn_cat (inode_state& state, const wordvec& words){
   if(words.size() > 1) {
      for (auto word = words.begin()+1; word != words.end(); word++){
         bool r = ((*word)[0] == '/');
         cout << state.readfile(split(*word,"/"),r) << endl;
      }
   }
   else {
      throw yshell_exn("cat:: no file given");
   }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words){
   for (auto iter =  words.begin()+1; iter != words.end(); iter++){
      cout << *iter << " ";
   }
   cout << "\n";
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit_exn();
}

//DONEDONEDONEDONEDONEDONEDONE///////////////////////////////////////
void fn_ls (inode_state& state, const wordvec& words){
   bool r = false;
   string orig = pathname(state.get_contents().at("."),
                  state.get_root()->get_inode_nr());
   if (words.size() > 1) {
      for(auto word = words.begin()+1; word != words.end(); word++) {
         if((*word)[0] == '/') r = true;
         state.set_cwd(split(*word,"/"), r);
         print_dir(state);
         state.set_cwd(split(orig,"/"),true);
      }
   }
   else
      print_dir(state);         

   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

//DONEDONEDONEDONEDONEDONE
void fn_make (inode_state& state, const wordvec& words){
   bool r = false;
   wordvec data;
   if(words[1][0] == '/') r = true;
   wordvec pathname = split(words[1],"/");
   for (auto iter = words.begin() + 2; iter != words.end(); iter++){
      data.push_back(*iter);
   }
   state.add_file(data,pathname,r);
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_prompt (inode_state& state, const wordvec& words){
   string str {};
   for (auto iter = words.begin()+1; iter != words.end(); ++iter){
      str = str + *iter + " ";
   }
   state.set_prompt(str);
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

string pathname(inode_ptr ip, int root_nr) {
   int my_inode_nr = ip->get_inode_nr();
   vector<string> pathname;
   while (my_inode_nr != root_nr) {
      ip = ip->get_contents().at("..");
      for(auto& elem: ip->get_contents()) {
         if(elem.second->get_inode_nr() == my_inode_nr) {
            pathname.insert(pathname.begin(), elem.first);
         }
      }
      my_inode_nr = ip->get_inode_nr();
   }

   string path_str = "/";
   for(auto& str : pathname) {
      path_str = path_str + str + "/";
   }
   return path_str;
}

void print_dir(inode_state& state) {
   cout << pathname(state.get_contents().at("."),
            state.get_root()->get_inode_nr()) << ":" << endl;
   for(auto elem : state.get_contents()) {
      cout << "\t" << elem.second->get_inode_nr() << " "
      << elem.second->size() << " "
      << elem.first << " "
      << endl;
   }
   cout << endl;
}
