#!/usr/bin/env python

# Tested with a Python 2.7.6

import os
import sys
import re
from urlparse import urlparse

SCRIPT_NAME = "logstat.py"
ONSITE = "example.com"
GIGA = 1000000000

class ApacheLogStat:
    def __init__(self):
        self.clean()


    def clean(self):
        '''Clean all previous records. '''
        self.totalRequests = 0
        self.onSiteRequests = 0
        self.customerStat = {}
        self.urlStat = {}


    def _addStatItem(self, customer, path, status, size, onSite):
        ''' Save parsed values of single line. '''
        # customers
        if self.customerStat.has_key(customer):
            self.customerStat[customer] += int(size)
        else:
            self.customerStat[customer] = int(size)

        # urls - keep stat only for 2xx responses
        s = int(status)
        if s >= 200 and s < 300:
            if self.urlStat.has_key(path):
                self.urlStat[path] += 1
            else:
                self.urlStat[path] = 1

        # off-size X on-site requests
        self.totalRequests += 1
        if onSite:
            self.onSiteRequests += 1


    def _parseRequest(self, request):
        ''' Parse request line starting with GET/POST/... and return customer name
            (dirname) and whole path. '''

        try:
            path = request.split()[1]
            company = path.split('/')[1]
        except:
            sys.stderr.write("Unexpected format of '{}'\n".format(request))
            raise

        return company, path


    def _isOnSite(self, referrer):
        try:
            parse = urlparse(referrer)
            regex = ".*{}".format(ONSITE)
            match = re.match(regex, parse.hostname)
        except:
            sys.stderr.write("Unexpected format of '{}'\n".format(referrer))
            raise

        return True if match else False


    def _processLine(self, line):
        ''' Parse line '''
        match = re.match(r'(.*) - - \[(.*)\] \"(.*?)\" ([0-9]+) ([0-9]+) \"(.*?)\" (.*)', line)

        if match == None:
            sys.stderr.write("Unexpected format of log line '{}'\n".format(line))
            raise RuntimeError('Error while parsing log line.')

        groups = match.groups();

        customer, path = self._parseRequest(groups[2])
        status = groups[3]
        size = groups[4]
        onSite = self._isOnSite(groups[5])

        #print "{} - {} - {} - {} - {}".format(customer, path, status, size, onSite)
        self._addStatItem(customer, path, status, size, onSite)


    def _procesFile(self, file):
        ''' Process line by line from file. '''
        for line in file:
            self._processLine(line)


    def addFile(self, path):
        ''' Add single file and process its content. '''
        try:
            with open(path) as file:
                self._procesFile(file)
        except IOError:
            sys.stderr.write("Unable to process file '{}'.".format(path))
            raise
        except:
            raise


    def printStat(self):
        ''' Print statistics. '''
        # headline
        offSite = self.totalRequests - self.onSiteRequests
        print "Off-site requests: {} of {} ({:.2f}%)".format(
            offSite,
            self.totalRequests,
            (float(offSite) / float(self.totalRequests)) * 100
        )

        # top 10 urls
        print
        print "Top 10 URLs:"
        sortedUrl = sorted(self.urlStat, key=self.urlStat.get, reverse=True)
        for url in sortedUrl[:10]:
            print "\t{} - {}".format(
                self.urlStat[url],
                url
            )

        # customers summary
        print
        print "Customer usage summary:"
        for key in self.customerStat.keys():
            print "\t{:.2f} GB - {}".format(
                float(self.customerStat[key]) / float(GIGA),
                key
            )


def usage():
    sys.stderr.write('Usage: {} <log folder>\n'.format(SCRIPT_NAME))


def main(argv):
    ''' Main function - get argv and process it. '''
    if len(argv) == 0:
        usage()
        return 1

    logStat = ApacheLogStat();

    try:
        # I don't expect any subdirectory or other files (that's how I
        # understdood requirements section)
        listDir = os.listdir(argv[0])
        for filename in listDir:
            logStat.addFile(os.path.join(argv[0], filename))
    except OSError, e:
        sys.stderr.write(str(e) + "\n")
        usage()
        return 1
    except:
        sys.stderr.write("Statistics couldn't be processed.\n")
        return 1

    logStat.printStat()

    return 0


if __name__ == "__main__":
    rc = main(sys.argv[1:])
    sys.exit(rc)
