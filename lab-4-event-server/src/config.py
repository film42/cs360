class Config:

    def __init__(self, path):
        self.config_path = path
        self.media = {}
        self.hosts = {}
        self.parameters = {}

        self.parse()

    def timeout(self):
        if 'timeout' in self.parameters:
            return int(self.parameters['timeout'])

    def content_type(self, path):
        extension = path.split('.')[-1]

        if extension in self.media:
            return self.media[extension]
        else:
            return 'text/plain'

    def absolute_path(self, relative_path, host='default'):
        last_rel_char = relative_path[-1]

        abs_path = self.hosts[host] + relative_path

        if last_rel_char == '/':
            abs_path += 'index.html'

        return abs_path

    def parse(self):
        with open(self.config_path, 'r') as f:
            contents = f.read()
        lines = contents.split("\n")

        for line in lines:
            try:
                klass, name, value = line.split()

                # Switch on the type
                if klass == 'host':
                    self.hosts[name] = value
                elif klass == 'media':
                    self.media[name] = value
                elif klass == 'parameter':
                    self.parameters[name] = value
                else:
                    raise Exception("Unrecognized parameter class")

            # Could not parse the line
            except:
                continue