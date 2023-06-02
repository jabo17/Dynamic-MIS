import os

instances = ['amazon-ratings',
'citeulike_ui',
'dewiki_clean',
'dnc-temporalGraph',
'facebook-wosn-wall',
'flickr-growth',
'haggle',
'lastfm_band',
'lkml-reply',
'movielens10m',
'munmun_digg',
'proper_loans',
'sociopatterns-infections',
'stackexchange-stackoverflow',
'topology',
'uk',
'wikipedia-growth',
'wiki_simple_en',
'youtube-u-growth']

runs = 3

algo = "dytwoswap"

job_pattern = "./deploy/bin/{} {} {} > output/{}_{}_r{}.log"


job_filename = "jobs.txt"

jobs = []
for instance in instances:
    updates = 0
    with open("Inst/{}.inst".format(instance), "r") as update_file:
        updates = len(update_file.readlines())

    for r in range(runs): 
        jobs.append(job_pattern.format(algo, instance, updates, algo, instance, r) + "\n")

with open(job_filename, "w") as job_file:
    job_file.writelines(jobs)

