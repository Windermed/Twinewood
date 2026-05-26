@echo off
cd WinderEngine
git pull
cd ..
git add WinderEngine
git commit -m "Update WinderEngine submodule"
git push