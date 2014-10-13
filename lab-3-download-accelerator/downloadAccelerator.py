from threadedDownload import Downloader
import argparse

def parse_arguments():
    parser = argparse.ArgumentParser(prog='Download Accelerator', description='CS360 Lab 3', add_help=True)
    parser.add_argument('-n', '--threads', type=int, action='store', help='Thread Count',default=10)
    parser.add_argument('url', metavar='url', type=str, help='URL to download')
    return parser.parse_args()

if __name__ == '__main__':
    args = parse_arguments()

    if args.threads == 0:
        raise Exception('Error: Thread count cannot be zero')

    d = Downloader(args.threads, args.url)
    d.start()