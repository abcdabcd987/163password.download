```bash
git clone https://github.com/google/leveldb.git
cd leveldb
make
sudo cp out-static/*.a /usr/local/lib/
sudo cp -r include/leveldb /usr/local/include/
sudo ldconfig
cd ..

./data_import.sh

npm install levelup leveldown
npm install -g forever
forever start web.js
```
