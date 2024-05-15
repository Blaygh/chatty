# Use a base image with GCC
FROM gcc:latest

# Set the working directory inside the container
WORKDIR /app

# Install necessary Boost libraries
RUN apt-get update && apt-get install -y libboost-system-dev libboost-thread-dev

# Copy the source code into the container
COPY bserver.cpp /app
COPY pdffilesend.pdf /app

# Build the bserver program
RUN g++ -o bserver bserver.cpp -lboost_system -lboost_thread -lpthread

# Specify the command to run your program when the container starts
CMD ["./bserver"]