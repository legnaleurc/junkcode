#! /bin/sh

zipcmd=`which zip`
jarcmd=`which jar`

if [ -z ${zipcmd} ] ; then
    echo 'Error: require zip, exit with error code ( 1 ).'
    exit 1
fi
if [ -z ${jarcmd} ] ; then
    echo 'Error: require jar, exit with error code ( 1 ).'
    exit 1
fi

cp -Rv src .tmp
cd .tmp
find . -name .svn | xargs rm -rvf
cd chrome
rm -vf *.jar
${jarcmd} -cvf rubino.jar *
cd ..
${zipcmd} -v ../Rubino.xpi chrome/rubino.jar install.rdf chrome.manifest
cd ..
rm -rvf .tmp
