#!/usr/bin/python3
import argparse
import datetime
import json
import os
import random
import rospy
import std_msgs.msg
from std_msgs.msg import String
from std_msgs.msg import UInt8
from sensor_msgs.msg import Imu

MSG_PER_SECOND = 10

DRV_MSG_LEN = 63  # space for the zero byte at the end for the client

def callback_imu_data(msg):
    rospy.logdebug("Received data: %s", msg)


def create_lra_data_msg(intensity):
    data_dict = {}
    for i in range(5):
        data_dict.setdefault(i, intensity)
    jsonstring = json.dumps(data_dict)
    msg_len = len(jsonstring)
    if msg_len < DRV_MSG_LEN:
        jsonstring += " " * (DRV_MSG_LEN - msg_len)
    return jsonstring


class SequenzTestNode:
    def __init__(self, levels, file_name):
        intesity_levels_3=[165,200,255]
        intesity_levels_5=[165,185,200,220,255]
        timestamp = "_"+datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
        self.playlist_indices = []
        self.filename = file_name + timestamp + ".txt"
        self.playlist = []
        self.levels = levels
        rospy.init_node('haptic_feedback_sequenz_tester', anonymous=True)
        self.pub = rospy.Publisher('/haptic_feedback/lra_motor_array', String, queue_size=MSG_PER_SECOND)
        self.rate = rospy.Rate(MSG_PER_SECOND)
        self.counter = 0
        for i in range(levels):
            for a in range(10):
                self.playlist_indices.append(i)
        print(self.playlist_indices)
        random.shuffle(self.playlist_indices)
        print(self.playlist_indices)
        for i in self.playlist_indices:
            if levels == 3:
                self.playlist.append(intesity_levels_3[i])
            elif levels == 5:
                self.playlist.append(intesity_levels_5[i])

    def run(self):
        for i in self.playlist:
            for _ in range(5*MSG_PER_SECOND):
                try:
                    msg = create_lra_data_msg(i)
                    rospy.loginfo("Sending data is: %s", msg)
                    self.pub.publish(msg)
                    self.rate.sleep()
                except Exception as e:
                    rospy.logerr(e)
            for _ in range(3*MSG_PER_SECOND):
                try:
                    msg = create_lra_data_msg(127)
                    rospy.loginfo("Sending data is: %s", msg)
                    self.pub.publish(msg)
                    self.rate.sleep()
                except Exception as e:
                    rospy.logerr(e)
        rospy.loginfo("Finished playlist")
        report = "Level report:\n"
        for i, level in enumerate(self.playlist_indices):
            report += "run: "+str(i+1)+" intensity: "+str(level+1)+"\n"
        rospy.loginfo(report)
        file = open(self.filename, "w")
        file.write(report)
        file.close()
        rospy.loginfo("Report written to file: "+self.filename + ". Path: " + os.path.abspath(self.filename))
        rospy.signal_shutdown("Finished playlist")


if __name__ == '__main__':
    try:
        parser = argparse.ArgumentParser(description='Run a haptic feedback test')
        parser.add_argument('--levels', type=int, default=3, help='Number of levels to test')
        parser.add_argument('--filename', type=str, default="test", help='Filename to write report to')

        args = parser.parse_args()

        node = SequenzTestNode(args.levels, args.filename)
        node.run()

    except rospy.ROSInterruptException:
        pass
