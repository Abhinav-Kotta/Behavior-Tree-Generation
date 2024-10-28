import fs from 'fs';
import path from 'path';
import { NextRequest, NextResponse } from 'next/server';

export async function POST(request: NextRequest) {
  const filePath = path.join(process.cwd(), '../scenarios.json');
  
  try {
    // Get the request body as new data to be written
    const newData = await request.json();

    // Write the new data directly to the file, replacing any existing content
    fs.writeFileSync(filePath, JSON.stringify(newData, null, 2), 'utf-8');

    return NextResponse.json({ message: 'Data written successfully' }, { status: 200 });
  } catch (error) {a
    return NextResponse.json({ message: 'Error writing to file', error: error.message }, { status: 500 });
  }
}
