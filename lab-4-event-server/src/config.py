class Config:

    def __init__(self, path):
        self.config_path = path
        self.path = ""
        self.media = {}
        self.hosts = {}
        self.parameters = {}

        self.parse()

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
                    self.parameters[name] = int(value)

            # Could not parse the line
            except:
                continue