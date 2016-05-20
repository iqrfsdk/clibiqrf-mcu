#!/bin/bash -e
openssl aes-256-cbc -K $encrypted_4d23fe3cd75c_key -iv $encrypted_4d23fe3cd75c_iv -in ci/travis-ci_rsa.enc -out ci/travis-ci_rsa -d
chmod 0600 ci/travis-ci_rsa
cp ci/travis-ci_rsa ~/.ssh/id_rsa
