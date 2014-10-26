Lab 4 - Event Server
====================

# To Run (from the project root):

```
python src/web.py [-p 3000]
```

Note that this will grab the provided `web.conf` that exists in the root of the project directory as well.


# Testings (TAs Look here):

You can execute any of the tests, and they are in the `/test` folder.

```
# [Once the server has been started, you should be able to copy-paste these]

# Protocol Test
chmod ugo-r test/web/static/files/test.txt
python test/protocol.py -s localhost -p 3000

# Stress Test
python test/stress-test.py localhost:3000/static/files/largefile.txt -t 10 -d 10
python test/stress-test.py localhost:3000/static/files/largefile.txt -t 100 -d 10

# Generator Test
# Note: We're using the `gen` host now so we can point to the `test/www` directory
python test/generator.py --port 3000 -l 10 -d 30 -s gen
```
