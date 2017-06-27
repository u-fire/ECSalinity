#doxygen Doxyfile
#moxygen ./docs/xml/
#rm ./docs/xml/ -R
#mv api.md ../../doc/source/includes/
cd ../../doc/
bundle exec middleman build --clean
cd build
cp -R * ../../lib/ECSalinity/docs/
cd ../../lib/ECSalinity/
