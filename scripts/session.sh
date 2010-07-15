#!/bin/sh

save_session()
{
     rm -rf ~/.config/cream-browser/session

     buffers=$(creamctl -s $1 -c "buffers")
     for i in $buffers
     do
          echo $i >> ~/.config/cream-browser/session
     done
}

load_session()
{
     while read line
     do
          creamctl -s $1 -c "tabopen $line"
     done < ~/.config/cream-browser/session
}

case "$4" in
     "save") save_session $3 ;;
     "load") load_session $3 ;;
     *) ;;
esac
