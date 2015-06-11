#!/usr/bin/env python

import os
import sys
import re

from urlparse import urlparse

SCRIPT_NAME="logstat.py"
ONSITE="example.com"
GIGA=1000000000

class ApacheLogStat:
    def __init__(self):
        self.clean()

    def clean(self):
        '''Clean all. '''
        self.totalRequests = 0
        self.onSiteRequests = 0
        self.customerStat = {}
        self.urlStat = {}

    def _addStatItem(self, customer, path, status, size, onSite):

        if self.customerStat.has_key(customer):
            self.customerStat[customer] += size
        else:
            self.customerStat[customer] = size


        if self.urlStat.has_key(path):
            self.urlStat[path] += 1
        else:
            self.urlStat[path] = 1

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
            raise

        return company, path


    def _isOnSite(self, referrer):
        try:
            parse = urlparse(referrer)
            regex = ".*{}".format(ONSITE)
            match = re.match(regex, parse.hostname)
        except:
            raise

        return True if match else False


    def _processLine(self, line):
        ''' Parse line '''
        match = re.match(r'(.*) - - \[(.*)\] \"(.*?)\" ([0-9]+) ([0-9]+) \"(.*?)\" (.*)', line)

        if match == None:
            raise RuntimeError('Error while parsing log line') # FIXME

        groups = match.groups();

        customer, path = self._parseRequest(groups[2])
        status = groups[3]
        size = groups[4]
        onSite = self._isOnSite(groups[5])

        #print "{} - {} - {} - {} - {}".format(customer, path, status, size, onSite)
        self._addStatItem(customer, path, status, size, onSite)


    def _procesFile(self, file):
        ''' Open file and process each line. '''
        try:
            for line in file:
                self._processLine(line)
        except:
            raise


    def addFile(self, path):
        ''' Add single file and process its content. '''
        with open(path) as file:
            self._procesFile(file)


    def printStat(self):
        ''' Print statistics. '''
        offSite = self.totalRequests - self.onSiteRequests
        print "Off-site requests: {} of {} ({:.2f}%)".format(
            offSite,
            self.totalRequests,
            (float(offSite) / float(self.totalRequests)) * 100
        )

        print
        print "Top 10 URLs:"
        sortedUrl = sorted(self.urlStat, key=self.urlStat.get, reverse=True)
        for url in sortedUrl[:10]:
            print "\t{} - {}".format(
                self.urlStat[url],
                url
            )

        print
        print "Customer usage summary:"
        for key in self.customerStat.keys():
            print "\t{} {} GB - {}".format(
                float(self.customerStat[key]), float(GIGA),
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
        listDir = os.listdir(argv[0]) # FIXME - use glob?
        for filename in listDir:
            logStat.addFile(os.path.join(argv[0], filename))
    except:
        raise # FIXME

    logStat.printStat()

    return 0


if __name__ == "__main__":
    rc = main(sys.argv[1:])
    sys.exit(rc)
