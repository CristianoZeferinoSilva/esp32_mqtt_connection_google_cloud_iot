

Generate an Eliptic Curve (EC) private / public key pair:

    openssl ecparam -genkey -name prime256v1 -noout -out ec_private.pem
    openssl ec -in ec_private.pem -pubout -out ec_public.pem

Register the device using the keys you generated:

    gcloud iot devices create atest-dev  --region=us-central1 \
        --registry=atest-registry \
        --public-key path=ec_public.pem,type=es256

Replace values first:
* Project ID (get from console or `gcloud config list`)
* Location (default is `us-central1`)
* Registry ID (created in previous steps, e.g. `atest-reg`)
* Device ID (created in previous steps, e.g. `atest-device`)

You will also need to extract your private key using the following command:

    openssl ec -in ec_private.pem -noout -text

... and will need to copy the output for the private key bytes into the private
key string in your Arduino project on "private_key_str" variable .

Run the sample.