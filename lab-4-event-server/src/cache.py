class ResponseCache:

    def __init__(self):
        self.cache = {}

    def cache_for(self, fd):
        return fd in self.cache

    def set_cache(self, fd, cache):
        self.cache[fd] = cache

    def get_cache(self, fd):
        return self.cache[fd]

    def del_cache(self, fd):
        if fd in self.cache:
            del self.cache[fd]

    def keys(self):
        return self.cache.keys()