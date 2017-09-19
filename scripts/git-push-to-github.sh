#if [ "$HOSTNAME" = "main-1-X61s-64-bit" ]; then
   echo "We are on ${HOSTNAME} so let's push!"
   git push origin master
   git push --tags
#   git push -u origin bbb_porting
#else
#   echo "We are on ${HOSTNAME} and not on X61s"
#fi
