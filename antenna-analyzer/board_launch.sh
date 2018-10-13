#!/bin/bash
PROJDIR=$1
CONFIG=$2
BOARD=$3
DEBUG_HOST=$4
USER=$5
DEBUG_PORT=$6
DEBUG_PROGRAM=/opt/antenna-analyzer/bin/antenna-analyzer

/usr/bin/ssh-keygen -f "${HOME}/.ssh/known_hosts" -R ${DEBUG_HOST}

/usr/bin/rsync -avhP ${PROJDIR}/build-antenna-analyzer-${BOARD}-${CONFIG}/antenna-analyzer ${USER}@${DEBUG_HOST}:${DEBUG_PROGRAM}

/usr/bin/ssh -f ${USER}@${DEBUG_HOST} "sh -c 'killall gdbserver > /dev/null 2>&1'"
/usr/bin/ssh -f ${USER}@${DEBUG_HOST} "sh -c 'nohup gdbserver :${DEBUG_PORT} ${DEBUG_PROGRAM} > /dev/null 2>&1 &'"
