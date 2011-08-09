clear
build_it ()
{
  g++ main.cpp Socket.cpp Socket.h SocketException.h -o xentonpp
  echo " "
  echo "<=====================================>"
  echo Done building!
  echo "Build finished at: `date +%T` "
  echo "<=====================================>"
  echo " "
  echo "Bash 'xentonpp' to run the bot (./xentonpp)"
  echo "This bot was made in C++ by Lordofsraam from Flux-Net"
  echo "Forked by Contex and renamed to XentonPP (Xenton++)"
  echo " "
}
clear
echo Building the Xenton++ IRC bot...
build_it
fi