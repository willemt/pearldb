import os
import requests
import signal
import subprocess
import unittest
import time


pearl_exe = 'build/pearl'
pearl_url = 'http://127.0.0.1:8888'


class TestDatabase(unittest.TestCase):

    def drop_db(self, name):
        drop = subprocess.Popen('{0} drop -P {1}'.format(pearl_exe, name), shell=True, stderr=subprocess.STDOUT)
        drop.wait()

    def start_db(self, name):
        self.pearl_process = subprocess.Popen('{0} -P {1}'.format(pearl_exe, name), shell=True, stderr=subprocess.STDOUT)
        time.sleep(0.1)

    def setUp(self):
        self.drop_db('test_store')
        self.start_db('test_store')

    def tearDown(self):
        # self.pearl_process.kill()
        os.kill(self.pearl_process.pid, signal.SIGKILL)

    def test_put(self):
        target_url = '{0}/x/'.format(pearl_url)

        r = requests.put(target_url, 'xxx')
        self.assertEqual(r.status_code, 200)

        r = requests.get(target_url)
        self.assertEqual(r.status_code, 200)
        self.assertEqual(r.content, 'xxx')

    def test_get_non_existent_results_in_404(self):
        target_url = '{0}/x/'.format(pearl_url)
        r = requests.get(target_url)
        self.assertEqual(r.status_code, 404)
        self.assertEqual(r.content, '')


if __name__ == '__main__':
    unittest.main()
