import requests
from bs4 import BeautifulSoup
import sys
import re
import logging
import json
import threading
import time

logging.basicConfig(
    filename='app.log',
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)

current_url = None


def fetch_html(url):
    try:
        response = requests.get(url)
        response.raise_for_status()
        return response.text
    except requests.exceptions.RequestException as e:
        logging.error(f"Error fetching the HTML from {url}: {e}")
        return None


def extract_time(html):
    soup = BeautifulSoup(html, 'html.parser')
    time_data = []
    time_classes = ['hours', 'minutes', 'seconds']
    for time_class in time_classes:
        time_element = soup.find('span', {'class': time_class})
        if time_element:
            time_data.append(time_element.text.strip())

    if not time_data:
        time_element = soup.find('span', {'id': 'lbl-time'})
        if time_element:
            return time_element.text.strip()

    if not time_data:
        time_elements = soup.find_all(id=re.compile('.*time.*'))
        for element in time_elements:
            return element.text.strip()

    return ':'.join(time_data)


def send_time_to_server(hour, minute, second):
    url = "http://192.168.1.65:1234/setTime"
    data = {
        "h": hour,
        "m": minute,
        "sec": second
    }
    try:
        response = requests.post(url, json=data)
        response.raise_for_status()
        logging.info(f"Successfully sent time data: {data}")
        print(f"Successfully sent time data: {data}")
    except requests.exceptions.RequestException as e:
        logging.error(f"Error sending time data to server: {e}")
        print(f"Error sending time data to server: {e}")


def update_url():
    global current_url
    while True:
        new_url = input(
            "Please enter the URL (or press Enter to keep the current one): ").strip()
        if new_url:
            current_url = new_url


def fetch_and_send_time():
    while True:
        if current_url:
            html = fetch_html(current_url)
            if html:
                current_time = extract_time(html)
                print(f"Current time: {current_time}")
                logging.info(f"Successfully fetched time: {current_time}")

                time_parts = current_time.split(":")
                if len(time_parts) == 3:
                    hour, minute, second = time_parts
                    send_time_to_server(hour, minute, second)
                else:
                    print("Error: Unable to extract time in correct format")
                    logging.error("Unable to extract time in correct format")
        time.sleep(2)


if __name__ == "__main__":
    current_url = input("Please enter the initial URL: ").strip()

    url_thread = threading.Thread(target=update_url, daemon=True)
    url_thread.start()

    fetch_thread = threading.Thread(target=fetch_and_send_time, daemon=True)
    fetch_thread.start()

    while True:
        time.sleep(1)
