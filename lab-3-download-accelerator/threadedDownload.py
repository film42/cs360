import requests
import threading
import sys
import time

''' Downloader for a set of files '''
class Downloader:
    def __init__(self, thread_count, url):
        # Add one so ranges and divison works
        self.thread_count = thread_count + 1
        self.url = url
        self.threads = []
        self.timer = time.time()
        self.bytes = 0

    def start(self):
        self.download()
        self.save()
        self.stats()

    def stats(self):
        duration = int(time.time() - self.timer)
        # [URL] [threads] [bytes] [seconds]
        print "%s %d %d %d" % (self.url, self.thread_count-1, self.bytes, duration)

    def filename(self):
        if self.url[-1] == '/':
            return 'index.html'
        else:
            return self.url.split('/')[-1]

    def save(self):
        # filename
        filename = self.filename()

        # Perform work on the threads
        for t in self.threads:
            t.start()

        # Clear the file if it exists
        with open(filename, 'w') as f:
            f.write('')

        # Join them together in order
        for t in self.threads:
            t.join()
            with open(filename, 'a') as f:
                f.write(t.value())

    def download(self):
        # Get the head request
        head = requests.head(self.url)

        # Check for valid header
        if 'content-length' not in head.headers:
            print "Error: No content-length header for url %s" % self.url
            sys.exit(-1)

        # Break the bytes into chuncks
        bytes = int(head.headers['content-length'])
        self.bytes = bytes

        approximate_bytes_per_thread = bytes / self.thread_count

        byte_range = range(0, bytes, approximate_bytes_per_thread)
        byte_range[-1] = bytes-1 # Include the rest of the byte count - 1

        # If the range was not accurately sized
        if len(byte_range) > self.thread_count:
            # Merge last two elements and remove the last
            byte_range[-2] = byte_range[-1]
            del byte_range[-1]

        # Build the worker threads
        cursor = 0
        for chunck in byte_range[1:]: # skip first element

            d = DownThread(self.url, cursor, chunck)
            self.threads.append(d)

            # Advance the cursor
            cursor = chunck + 1

''' Use a thread to download one file given by url and stored in filename'''
class DownThread(threading.Thread):
    def __init__(self,url,start_bytes,end_bytes):
        self.url = url
        self.start_bytes = start_bytes
        self.end_bytes = end_bytes
        threading.Thread.__init__(self)
        self.content = None

    def value(self):
        return self.content

    def run(self):
        headers = {
            'range':           "bytes=%d-%d" % (self.start_bytes, self.end_bytes),
            'accept-encoding': 'identity'
        }
        r = requests.get(self.url, headers=headers)

        # Check for content range header
        if 'content-range' not in r.headers:
            print "Error: No content-range header for url %s" % self.url
            sys.exit(-1)

        self.content = r.content
