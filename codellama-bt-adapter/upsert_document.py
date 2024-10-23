import pdfplumber
from sentence_transformers import SentenceTransformer
from pinecone import Pinecone, ServerlessSpec
import os
from dotenv import load_dotenv

load_dotenv()

# Function to extract text from PDF
def extract_text_from_pdf(pdf_path):
    text = ""
    with pdfplumber.open(pdf_path) as pdf:
        for page in pdf.pages:
            text += page.extract_text()
    return text

# Function to chunk the extracted text
def chunk_text(text, chunk_size=500):
    return [text[i:i + chunk_size] for i in range(0, len(text), chunk_size)]

# Function to embed text chunks
def embed_chunks(chunks, model_name='all-MiniLM-L6-v2'):
    model = SentenceTransformer(model_name)
    embeddings = model.encode(chunks)
    return embeddings

# Function to initialize Pinecone and create an index
def init_pinecone(api_key, index_name, dimension=384):
    pc = Pinecone(api_key=api_key)
    if index_name not in pc.list_indexes():
        pc.create_index(
            name=index_name,
            dimension=dimension,
            metric="cosine",
            spec=ServerlessSpec(
                cloud="aws",
                region="us-east-1"
            )
        )
    return pc.Index(index_name)

# Function to upsert embeddings into Pinecone
def upsert_embeddings(index, embeddings, chunks, batch_size=100):
    for i in range(0, len(embeddings), batch_size):
        batch_vectors = []
        for j in range(i, min(i + batch_size, len(embeddings))):
            batch_vectors.append((str(j), embeddings[j].tolist(), {"text": chunks[j]}))
        index.upsert(batch_vectors)


# Main function to handle the entire process
def add_pdf_to_pinecone(pdf_path, pinecone_api_key, pinecone_index_name):
    extracted_text = extract_text_from_pdf(pdf_path)
    chunks = chunk_text(extracted_text)
    embeddings = embed_chunks(chunks)
    index = init_pinecone(pinecone_api_key, pinecone_index_name)
    upsert_embeddings(index, embeddings, chunks)
    print(f"Successfully added {len(chunks)} chunks from the PDF to Pinecone.")

# Define your variables
pdf_path = 'tank.pdf'  # Path to your PDF file
pinecone_api_key = os.getenv('PINECONE_API_KEY')  # Your Pinecone API key
pinecone_index_name = 'pdf-rag-index'  # Name of your Pinecone index

# Run the function to add the PDF to Pinecone
add_pdf_to_pinecone(pdf_path, pinecone_api_key, pinecone_index_name)
